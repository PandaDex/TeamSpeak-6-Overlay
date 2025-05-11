#include "websocketmanager.h"
#include <QDebug>
#include <QDateTime>

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
    content["apiKey"] = "";
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
        QPixmap avatarPixmap(QString(":/avatars/%1.png").arg(client.avatarUrl));
        UserBubble *bubble = new UserBubble(client.nickname, avatarPixmap, overlay);
        bubble->move(10, 10 + bubbles.size() * 40);
        bubble->show();
        bubbles[client.id] = bubble;
    }
}

void WebSocketManager::removeSpeakingClient(const QString &clientId)
{
    if (bubbles.contains(clientId)) {
        bubbles[clientId]->deleteLater();
        bubbles.remove(clientId);
    }
}
