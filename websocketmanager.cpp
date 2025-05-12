#include "WebSocketManager.h"
#include <QDebug>
#include <QDateTime>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

WebSocketManager::WebSocketManager(QWidget *overlayParent, QObject *parent)
    : QObject(parent), overlay(overlayParent)
{
    connect(&socket, &QWebSocket::connected, this, &WebSocketManager::onConnected);
    connect(&socket, &QWebSocket::textMessageReceived, this, &WebSocketManager::onTextMessageReceived);
}

void WebSocketManager::connectToServer()
{
    socket.open(QUrl("ws://localhost:5899/"));
}

void WebSocketManager::onConnected()
{
    qDebug() << "WebSocket connected";
    QJsonObject payload, content;

    QString apiKey = getApiKey();
    if (!apiKey.isEmpty()) {
        content["apiKey"] = apiKey;
    } else {
        content["apiKey"] = "";
        qDebug() << "No API key available for authentication";
    }

    payload["type"] = "auth";
    QJsonObject details;
    details["identifier"] = "dex.teamspeak6.overlay";
    details["version"] = "1.0.0";
    details["name"] = "Teamspeak Overlay";
    details["description"] = "Shows speaking clients on screen";
    details["content"] = content;
    payload["payload"] = details;

    socket.sendTextMessage(QJsonDocument(payload).toJson(QJsonDocument::Compact));
}

void WebSocketManager::onTextMessageReceived(QString message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject data = doc.object();
    QString type = data["type"].toString();
    QJsonObject payload = data["payload"].toObject();

    if (type == "auth") {
        clients.clear();
        QJsonArray infos = payload["connections"].toArray()[0].toObject()["clientInfos"].toArray();
        currentId = payload["connections"].toArray()[0].toObject()["clientId"].toString();
        currentChannel = payload["connections"].toArray()[0].toObject()["clientInfos"].toArray()
                             .first().toObject()["channelId"].toString().toInt();

        if(!payload["apiKey"].toString().isEmpty()) {
            saveApiKey(payload["apiKey"].toString());
        }

        qDebug() << payload;

        for (const QJsonValue &val : infos) {
            QJsonObject obj = val.toObject();
            ClientInfo info;
            info.id = obj["id"].toString();
            info.nickname = obj["properties"].toObject()["nickname"].toString();
            info.avatarUrl = obj["properties"].toObject()["myteamspeakAvatar"].toString().remove("2,");
            clients.append(info);
        }
    } else if (type == "talkStatusChanged") {
        QString clientId = payload["clientId"].toString();
        int status = payload["status"].toInt();

        auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientInfo &c) { return c.id == clientId; });
        if (it != clients.end()) {
            if (status == 1) {
                showSpeakingClient(*it);
            } else {
                removeSpeakingClient(it->id);
            }
        }
    }
}

void WebSocketManager::showSpeakingClient(const ClientInfo &client)
{
    if (!bubbles.contains(client.id)) {
        QString avatarUrl = client.avatarUrl;

        if (avatarUrl.isEmpty()) {
            qDebug() << "No avatar URL for client:" << client.nickname;
            return;
        }

        if (!avatarUrl.startsWith("http://") && !avatarUrl.startsWith("https://")) {
            avatarUrl = "https://" + avatarUrl;
        }

        QUrl url(avatarUrl);
        if (!url.isValid()) {
            qDebug() << "Invalid avatar URL:" << avatarUrl;
            return;
        }

        QNetworkRequest request(url);
        QNetworkReply* reply = networkManager.get(request);

        connect(reply, &QNetworkReply::finished, this, [=]() {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "Failed to load avatar from:" << avatarUrl << "Error:" << reply->errorString();
                reply->deleteLater();
                return;
            }

            QByteArray data = reply->readAll();
            QPixmap avatar;
            if (!avatar.loadFromData(data)) {
                qDebug() << "Failed to load avatar image data for:" << client.nickname;
                reply->deleteLater();
                return;
            }

            UserBubble *bubble = new UserBubble(client.nickname, avatar, overlay);
            bubble->move(10, 10 + bubbles.size() * 40);
            bubble->show();
            bubbles[client.id] = bubble;

            reply->deleteLater();
        });
    }
}

void WebSocketManager::removeSpeakingClient(const QString &clientId)
{
    if (bubbles.contains(clientId)) {
        bubbles[clientId]->deleteLater();
        bubbles.remove(clientId);
    }
}

void WebSocketManager::saveApiKey(const QString &apiKey)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("config.db");

    if (!db.open()) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS data (id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT)");
    if (!query.exec()) {
        qDebug() << "Failed to create table:" << query.lastError().text();
        return;
    }

    query.prepare("INSERT INTO data (key) VALUES (:key)");
    query.bindValue(":key", apiKey);
    if (!query.exec()) {
        qDebug() << "Failed to insert API key:" << query.lastError().text();
    }

    db.close();
}

QString WebSocketManager::getApiKey()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("config.db");

    if (!db.open()) {
        qDebug() << "Failed to open database";
        return QString();
    }

    QSqlQuery query("SELECT key FROM data ORDER BY id DESC LIMIT 1");
    QString apiKey;
    if (query.next()) {
        apiKey = query.value(0).toString();
    } else {
        qDebug() << "No API key found";
    }

    db.close();
    return apiKey;
}

