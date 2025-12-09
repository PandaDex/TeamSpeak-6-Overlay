#include "WebsocketManager.h"
#include "../core/ConfigManager.h"
#include "../core/Constants.h"
#include "../core/Logger.h"
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <algorithm>
#include "../ui/NewBubble.h"
#include <QRandomGenerator>

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

    LOG_INFO("Attempting to connect to WebSocket server...");
    socket.open(QUrl(Constants::WEBSOCKET_URL));
}

void WebSocketManager::onConnected()
{
    LOG_INFO("WebSocket connected");
    reconnectAttempts = 0;

    QJsonObject payload, content;

    QString apiKey = ConfigManager::get("apiKey");
    content["apiKey"] = apiKey.isEmpty() ? "" : apiKey;

    payload["type"] = "auth";
    QJsonObject details{
        {"identifier", Constants::APP_IDENTIFIER},
        {"version", Constants::APP_VERSION},
        {"name", Constants::APP_NAME},
        {"description", Constants::APP_DESCRIPTION},
        {"content", content}
    };
    payload["payload"] = details;

    socket.sendTextMessage(QJsonDocument(payload).toJson(QJsonDocument::Compact));

    int position = ConfigManager::get("overlayPosition").toInt();
    ALG_BOTTOM = position / 2;
    ALG_RIGHT = position % 2;
}

void WebSocketManager::onDisconnected()
{
    LOG_INFO("WebSocket disconnected.");
    QString apiKey = ConfigManager::get("apiKey");
    if (!apiKey.isEmpty()) {
        LOG_INFO("Connection closed after authentication, resetting API key.");
        ConfigManager::set("apiKey", "");
    }

    scheduleReconnect();
}

void WebSocketManager::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    LOG_ERROR("WebSocket error: " + socket.errorString());
    socket.abort();

    scheduleReconnect();
}

void WebSocketManager::scheduleReconnect()
{
    int delay = qMin(Constants::MAX_RECONNECT_DELAY_MS, Constants::BASE_RECONNECT_DELAY_MS * (1 << reconnectAttempts));
    LOG_INFO("Reconnecting in: " +  delay / 1000);
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

    LOG_DEBUG("Received message of type: " + type);

    if (type == "auth") {
        clients.clear();

        QJsonArray connections = payload["connections"].toArray();
        if (connections.isEmpty()) return;

        QJsonObject connection = connections[0].toObject();
        currentId = connection["clientId"].toString();
        QJsonArray infos = connection["clientInfos"].toArray();
        if (!infos.isEmpty()) {
            currentChannel = infos.first().toObject()["channelId"].toInt();
        }

        QString newApiKey = payload["apiKey"].toString();
        if (!newApiKey.isEmpty()) {
            ConfigManager::set("apiKey", newApiKey);
        }

        for (const QJsonValue &val : infos) {
            QJsonObject obj = val.toObject();
            ClientInfo info;

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
        if(props.isEmpty()) {LOG_WARNING("User props empty"); return;}

        bool exists = false;
        for (const ClientInfo& c : clients) {
            if (c.id == clientId) {
                exists = true;
                break;
            }
        }

        if (exists) {LOG_INFO("User is in"); return;}
        info.id = clientId;
        info.nickname = props["nickname"].toString();
        info.avatarUrl = props["myteamspeakAvatar"].toString().remove("2,");
        clients.append(info);
    }
    else if (type == "textMessage") {
        QJsonValue tempInvoker = payload["invoker"];
        QString clientName = tempInvoker["nickname"].toString();
        QString msg = payload["message"].toString();

        int rnd_id = QRandomGenerator::global()->bounded(100);

        LOG_DEBUG("Text message received from clientId: " + clientName + ", message: " + msg);

        if (!clientName.isEmpty()) {
            onUserMessageReceived(clientName, msg, rnd_id);
        } else {
            LOG_WARNING("Client not found for message: " + clientName);
        }
    }
}

void WebSocketManager::loadAvatarWithCache(const QString &avatarUrl, std::function<void(const QPixmap&)> callback)
{
    // Check if avatar is already cached
    if (avatarCache.contains(avatarUrl)) {
        LOG_INFO("Using cached avatar for URL: " + avatarUrl);
        callback(avatarCache[avatarUrl]);
        return;
    }

    if (pendingAvatarRequests.contains(avatarUrl)) {
        LOG_INFO("Avatar request already in progress for URL: " + avatarUrl);
        pendingAvatarCallbacks[avatarUrl].append(callback);
        return;
    }

    QString finalUrl = avatarUrl;
    if (!finalUrl.startsWith("http://") && !finalUrl.startsWith("https://")) {
        finalUrl = "https://" + finalUrl;
    }

    QUrl url(finalUrl);
    if (!url.isValid()) {
        LOG_WARNING("Invalid avatar URL: " + finalUrl);
        return;
    }

    LOG_INFO("Fetching avatar from URL: " + finalUrl);
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager.get(request);

    pendingAvatarRequests.insert(avatarUrl);
    pendingAvatarCallbacks[avatarUrl].append(callback);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        pendingAvatarRequests.remove(avatarUrl);

        if (reply->error() != QNetworkReply::NoError) {
            LOG_ERROR("Failed to load avatar from: " + finalUrl + " Error: " + reply->errorString());
            pendingAvatarCallbacks.remove(avatarUrl);
            return;
        }

        QByteArray data = reply->readAll();
        QPixmap avatar;
        if (!avatar.loadFromData(data)) {
            LOG_ERROR("Failed to load avatar image data");
            pendingAvatarCallbacks.remove(avatarUrl);
            return;
        }

        // Cache the avatar
        avatarCache[avatarUrl] = avatar;
        LOG_INFO("Avatar cached for URL: " + avatarUrl);

        if (pendingAvatarCallbacks.contains(avatarUrl)) {
            for (const auto &cb : pendingAvatarCallbacks[avatarUrl]) {
                cb(avatar);
            }
            pendingAvatarCallbacks.remove(avatarUrl);
        }
    });
}

void WebSocketManager::showSpeakingClient(const ClientInfo &client)
{
    if (bubbles.contains(client.id)) return;

    QString avatarUrl = Constants::DEFAULT_AVATAR_URL;
    LOG_INFO("avatars disabled. Using fallback");

    loadAvatarWithCache(avatarUrl, [=](const QPixmap &avatar) {
        bool r = false;
        if(ConfigManager::get("overlayPosition") == "1" || ConfigManager::get("overlayPosition") == "3") r = true;
        NewBubble *bubble = new NewBubble(client.nickname, avatar, nullptr,r, overlay);
        bubble->adjustSize();
        bubble->show();
        bubbles[client.id] = bubble;
    });
}

void WebSocketManager::onUserMessageReceived(QString clientName, QString message, int id)
{
    LOG_INFO("Displaying message from: " + clientName + " | message: " + message);
    QString avatarUrl = Constants::DEFAULT_AVATAR_URL;

    LOG_INFO("avatars disabled. Using fallback");

    loadAvatarWithCache(avatarUrl, [=](const QPixmap &avatar) {
        bool r = false;
        if(ConfigManager::get("overlayPosition") == "1" || ConfigManager::get("overlayPosition") == "3") r = true;
        NewBubble *msgBubble = new NewBubble(clientName, avatar, message, r, overlay);
        msgBubble->adjustSize();
        msgBubble->show();

        QString idS = QString::number(id);
        messageBubbles[clientName + message + idS] = msgBubble;

        QTimer::singleShot(6000, this, [this, clientName, message, idS]() {
            if (messageBubbles.contains(clientName + message + idS)) {
                messageBubbles[clientName + message + idS]->deleteLater();
                messageBubbles.remove(clientName + message + idS);
            }
        });
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
    LOG_INFO("Force reconnect called.");
    socket.abort();
    reconnectAttempts = 0;
}
