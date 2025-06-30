#include "WebSocketManager.h"
#include "databasemanager.h"
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <algorithm>

WebSocketManager* WebSocketManager::m_instance = nullptr;

WebSocketManager* WebSocketManager::instance()
{
    return m_instance;
}

WebSocketManager::WebSocketManager(QWidget *overlayParent, QObject *parent)
    : QObject(parent), overlay(overlayParent), reconnectAttempts(0)
{
    m_instance = this;
    connect(&socket, &QWebSocket::connected, this, &WebSocketManager::onConnected);
    connect(&socket, &QWebSocket::textMessageReceived, this, &WebSocketManager::onTextMessageReceived);
    connect(&socket, &QWebSocket::disconnected, this, &WebSocketManager::onDisconnected);
    connect(&socket, &QWebSocket::errorOccurred, this, &WebSocketManager::onError);
}

void WebSocketManager::connectToServer()
{
    if (socket.state() == QAbstractSocket::ConnectedState || socket.state() == QAbstractSocket::ConnectingState)
        return;

    qDebug() << "Attempting to connect to WebSocket server...";
    socket.open(QUrl("ws://localhost:5899/"));
}

void WebSocketManager::onConnected()
{
    qDebug() << "WebSocket connected";
    reconnectAttempts = 0;

    QJsonObject payload, content;

    QString apiKey = DatabaseManager::get("apiKey");
    content["apiKey"] = apiKey.isEmpty() ? "" : apiKey;

    payload["type"] = "auth";
    QJsonObject details{
        {"identifier", "dex.teamspeak6.overlay"},
        {"version", "1.0.0"},
        {"name", "Teamspeak Overlay"},
        {"description", "Shows speaking clients on screen"},
        {"content", content}
    };
    payload["payload"] = details;

    socket.sendTextMessage(QJsonDocument(payload).toJson(QJsonDocument::Compact));

    int position = DatabaseManager::get("overlayPosition").toInt();
    WebSocketManager::ALG_BOTTOM = position / 2;
    WebSocketManager::ALG_RIGHT = position % 2;
}

void WebSocketManager::onDisconnected()
{
    qWarning() << "WebSocket disconnected.";
    QString apiKey = DatabaseManager::get("apiKey");
    if (!apiKey.isEmpty()) {
        qInfo() << "Connection closed after authentication, resetting API key.";
        DatabaseManager::set("apiKey", "");
    }

    scheduleReconnect();
}

void WebSocketManager::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    qWarning() << "WebSocket error:" << socket.errorString();
    socket.abort();

    scheduleReconnect();
}

void WebSocketManager::scheduleReconnect()
{
    int delay = qMin(30000, 1000 * (1 << reconnectAttempts));
    qDebug() << "Reconnecting in" << delay / 1000 << "seconds...";
    QTimer::singleShot(delay, this, &WebSocketManager::connectToServer);
    reconnectAttempts++;
}

void WebSocketManager::onTextMessageReceived(QString message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject data = doc.object();
    QString type = data["type"].toString();
    QJsonObject payload = data["payload"].toObject();

    qDebug() << "Received message of type:" << type;

    if (type == "auth") {
        clients.clear();

        QJsonArray connections = payload["connections"].toArray();
        if (connections.isEmpty()) return;

        QJsonObject connection = connections[0].toObject();
        qDebug() << connection;
        currentId = connection["clientId"].toString();
        QJsonArray infos = connection["clientInfos"].toArray();
        if (!infos.isEmpty()) {
            currentChannel = infos.first().toObject()["channelId"].toInt();
        }

        QString newApiKey = payload["apiKey"].toString();
        if (!newApiKey.isEmpty()) {
            DatabaseManager::set("apiKey", newApiKey);
        }

        for (const QJsonValue &val : infos) {
            QJsonObject obj = val.toObject();
            ClientInfo info;
            qDebug() << obj["id"];

            info.id = QString::number(obj["id"].toDouble());
            QJsonObject props = obj["properties"].toObject();
            info.nickname = props["nickname"].toString();
            info.avatarUrl = props["myteamspeakAvatar"].toString().remove("2,");
            clients.append(info);
        }
    } else if (type == "talkStatusChanged") {
        QString clientId = QString::number(payload["clientId"].toDouble());
        int status = payload["status"].toInt();

        auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientInfo &c) { return c.id == clientId; });
        if (it != clients.end()) {
            if (status == 1) {
                showSpeakingClient(*it);
            } else {
                removeSpeakingClient(it->id);
            }
        }
    } else if (type == "clientMoved") {
        QString clientId = QString::number(payload["clientId"].toDouble());
        QJsonObject props = payload["properties"].toObject();
        ClientInfo info;
        if(props.isEmpty()) {qDebug() << "User props empty"; return;}

        bool exists = false;
        for (const ClientInfo& c : clients) {
            if (c.id == clientId) {
                exists = true;
                break;
            }
        }

        if (exists) {qDebug() << "User is in"; return;}
        info.id = clientId;
        info.nickname = props["nickname"].toString();
        info.avatarUrl = props["myteamspeakAvatar"].toString().remove("2,");
        clients.append(info);
    }
}

void WebSocketManager::showSpeakingClient(const ClientInfo &client)
{
    if (bubbles.contains(client.id)) return;

    //temp
    QString avatarUrl ="https://raw.githubusercontent.com/PandaDex/TeamSpeak-6-Overlay/refs/heads/master/resources/icon%401x.ico";
    qDebug() << "avatars disabled. Using fallback";


    // QString avatarUrl = client.avatarUrl;
    // if (avatarUrl.isEmpty()) {
    //     qDebug() << "USING DEFAULT FALLBACK! No avatar URL for client:" << client.nickname << "got" << avatarUrl;
    //     avatarUrl = "https://raw.githubusercontent.com/PandaDex/TeamSpeak-6-Overlay/refs/heads/master/resources/icon%401x.ico";
    // }

    if (!avatarUrl.startsWith("http://") && !avatarUrl.startsWith("https://")) {
        avatarUrl = "https://" + avatarUrl;
    }

    QUrl url(avatarUrl);
    if (!url.isValid()) {
        qDebug() << "Invalid avatar URL:" << avatarUrl;
        return;
    }

    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager.get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Failed to load avatar from:" << avatarUrl << "Error:" << reply->errorString();
            return;
        }

        QByteArray data = reply->readAll();
        QPixmap avatar;
        if (!avatar.loadFromData(data)) {
            qDebug() << "Failed to load avatar image data for:" << client.nickname;
            return;
        }

        UserBubble *bubble = new UserBubble(client.nickname, avatar, overlay);
        bubble->adjustSize();

        int x = WebSocketManager::ALG_RIGHT ? overlay->width() - bubble->width() - 10 : 10;
        int y = WebSocketManager::ALG_BOTTOM ? overlay->height() - bubble->height() - 10 - bubbles.size() * 30 : 10 + bubbles.size() * 30;

        bubble->move(x, y);
        bubble->show();
        bubbles[client.id] = bubble;
    });
}

void WebSocketManager::removeSpeakingClient(const QString &clientId)
{
    if (bubbles.contains(clientId)) {
        bubbles[clientId]->deleteLater();
        bubbles.remove(clientId);
    }
}

void WebSocketManager::forceReconnect()
{
    qDebug() << "Force reconnect called.";
    socket.abort();
    reconnectAttempts = 0;
}
