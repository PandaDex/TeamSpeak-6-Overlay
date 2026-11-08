#include "WebsocketManager.h"
#include "../core/ConfigManager.h"
#include "../core/Constants.h"
#include "../core/Logger.h"
#include "../core/OverlayPosition.h"
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
    connect(&socket, &QWebSocket::binaryMessageReceived, this, [](const QByteArray &m) { LOG_DEBUG("BINARY(" + QString::number(m.size()) + "): " + QString::fromUtf8(m.left(400))); }); // TEMP-DEBUG

    // A single reconnect timer, so error + disconnected signals for the same
    // failure can't stack up multiple reconnect attempts.
    reconnectTimer.setSingleShot(true);
    connect(&reconnectTimer, &QTimer::timeout, this, &WebSocketManager::connectToServer);

    connect(ConfigManager::instance(), &ConfigManager::configChanged, this, &WebSocketManager::onConfigChanged);
}

QString WebSocketManager::idToString(const QJsonValue &value)
{
    // TeamSpeak sends ids sometimes as numbers and sometimes as strings
    if (value.isString()) return value.toString();
    if (value.isDouble()) return QString::number(static_cast<qint64>(value.toDouble()));
    return QString();
}

QPixmap WebSocketManager::defaultAvatar()
{
    if (m_defaultAvatar.isNull()) {
        m_defaultAvatar.load(Constants::DEFAULT_AVATAR_RESOURCE);
        if (m_defaultAvatar.isNull()) {
            m_defaultAvatar = QPixmap(Constants::DEFAULT_AVATAR_SIZE, Constants::DEFAULT_AVATAR_SIZE);
            m_defaultAvatar.fill(Qt::darkGray);
        } else {
            m_defaultAvatar = m_defaultAvatar.scaled(Constants::DEFAULT_AVATAR_SIZE * 2, Constants::DEFAULT_AVATAR_SIZE * 2,
                                                     Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
    return m_defaultAvatar;
}

bool WebSocketManager::isChannelMode() const
{
    return DisplayMode::fromString(ConfigManager::get("displayMode")) == DisplayMode::WholeChannel;
}

bool WebSocketManager::isRightAligned() const
{
    return OverlayPosition::isRight(OverlayPosition::fromString(ConfigManager::get("overlayPosition")));
}

void WebSocketManager::connectToServer()
{
    if (socket.state() == QAbstractSocket::ConnectedState || socket.state() == QAbstractSocket::ConnectingState)
        return;

    LOG_INFO("Attempting to connect to WebSocket server...");
    socket.open(QUrl(qEnvironmentVariableIsSet("TSO_WS_URL") ? qEnvironmentVariable("TSO_WS_URL") : Constants::WEBSOCKET_URL)); // TEMP-DEBUG
}

void WebSocketManager::onConnected()
{
    LOG_INFO("WebSocket connected");
    reconnectAttempts = 0;
    authenticated = false;

    QJsonObject payload, content;

    QString apiKey = ConfigManager::get("apiKey");
    sentStoredApiKey = !apiKey.isEmpty();
    content["apiKey"] = apiKey;

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

    // TeamSpeak stays silent (but keeps streaming events) when it knows this
    // app but doesn't accept the key we sent, e.g. after the key was lost.
    QTimer::singleShot(15000, this, [this]() {
        if (socket.state() == QAbstractSocket::ConnectedState && !authenticated) {
            LOG_WARNING("No auth reply from TeamSpeak after 15s. Accept the overlay in TeamSpeak, or remove "
                        "it under Settings > Remote Apps and reconnect so TeamSpeak issues a new API key.");
        }
    });
}

void WebSocketManager::onDisconnected()
{
    LOG_INFO("WebSocket disconnected.");

    if (!authenticated && sentStoredApiKey) {
        LOG_WARNING("Connection closed before authentication completed, API key was probably rejected. Resetting it.");
        ConfigManager::set("apiKey", "");
    }
    authenticated = false;
    sentStoredApiKey = false;

    clearClientBubbles();
    clients.clear();
    speakingClients.clear();
    currentId.clear();
    currentChannel.clear();

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
    if (reconnectTimer.isActive()) return;

    int delay = qMin(Constants::MAX_RECONNECT_DELAY_MS,
                     Constants::BASE_RECONNECT_DELAY_MS * (1 << qMin(reconnectAttempts, 5)));
    LOG_INFO("Reconnecting in: " + QString::number(delay / 1000) + "s");
    reconnectTimer.start(delay);
    reconnectAttempts++;
}

void WebSocketManager::onTextMessageReceived(QString message)
{
    LOG_DEBUG("RAW(" + QString::number(message.size()) + "): " + message.left(400)); // TEMP-DEBUG
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);
    if (!doc.isObject()) {
        LOG_WARNING("Ignoring unparseable message (" + QString::number(message.size()) + " chars): "
                    + parseError.errorString() + " at offset " + QString::number(parseError.offset));
        return;
    }

    QJsonObject data = doc.object();
    QString type = data["type"].toString();
    QJsonObject payload = data["payload"].toObject();

    LOG_DEBUG("Received message of type: " + type);

    if (type == "auth") {
        handleAuth(payload);
    } else if (type == "talkStatusChanged") {
        handleTalkStatusChanged(payload);
    } else if (type == "clientMoved") {
        handleClientMoved(payload);
    } else if (type == "textMessage") {
        handleTextMessage(payload);
    } else if (type == "clientPropertiesUpdated") {
        handleClientPropertiesUpdated(payload);
    } else if (type == "clientSelfPropertyUpdated") {
        // Some TeamSpeak builds never send the "auth" reply, so our own client id
        // is unknown. Our own talk flag change is immediately followed by the
        // matching talkStatusChanged for our id: remember it to correlate.
        if (currentId.isEmpty() && payload["flag"].toString() == "flagTalking") {
            pendingSelfTalk = payload["newValue"].toBool() ? 1 : 0;
            pendingSelfTalkTimer.start();
        }
    } else if (type == "connectStatusChanged") {
        // Connected to a server after the overlay authenticated: learn our own id
        QString selfId = idToString(payload["info"].toObject()["clientId"]);
        if (!selfId.isEmpty() && selfId != currentId) {
            currentId = selfId;
            auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientInfo &c) { return c.id == currentId; });
            if (it != clients.end() && !it->channelId.isEmpty()) {
                currentChannel = it->channelId;
                rebuildClientBubbles();
            }
        }
    }
}

void WebSocketManager::handleAuth(const QJsonObject &payload)
{
    authenticated = true;

    QString newApiKey = payload["apiKey"].toString();
    if (!newApiKey.isEmpty() && newApiKey != ConfigManager::get("apiKey")) {
        if (!ConfigManager::set("apiKey", newApiKey)) {
            LOG_ERROR("Failed to save API key, TeamSpeak will ask again on next start.");
        }
    }

    clearClientBubbles();
    clients.clear();
    speakingClients.clear();
    currentId.clear();
    currentChannel.clear();

    QJsonArray connections = payload["connections"].toArray();
    if (connections.isEmpty()) return;

    QJsonObject connection = connections[0].toObject();
    currentId = idToString(connection["clientId"]);
    QJsonArray infos = connection["clientInfos"].toArray();

    for (const QJsonValue &val : infos) {
        QJsonObject obj = val.toObject();
        ClientInfo info;

        info.id = idToString(obj["id"]);
        info.channelId = idToString(obj["channelId"]);
        QJsonObject props = obj["properties"].toObject();
        info.nickname = props["nickname"].toString();
        info.avatarUrl = props["myteamspeakAvatar"].toString().remove("2,");
        clients.append(info);

        if (info.id == currentId) currentChannel = info.channelId;
    }

    if (currentChannel.isEmpty() && !clients.isEmpty()) {
        currentChannel = clients.first().channelId;
    }

    rebuildClientBubbles();
}

void WebSocketManager::handleTalkStatusChanged(const QJsonObject &payload)
{
    QString clientId = idToString(payload["clientId"]);
    bool speaking = payload["status"].toInt() == 1;

    if (currentId.isEmpty() && pendingSelfTalk == (speaking ? 1 : 0)
        && pendingSelfTalkTimer.isValid() && pendingSelfTalkTimer.elapsed() < 250) {
        currentId = clientId;
        LOG_INFO("Identified own client id from talk events: " + currentId);
    }
    pendingSelfTalk = -1;

    if (clientId == currentId && !clientId.isEmpty()
        && std::none_of(clients.begin(), clients.end(), [&](const ClientInfo &c) { return c.id == clientId; })) {
        // Our own properties only come with the auth reply; fall back to a label
        ClientInfo self;
        self.id = clientId;
        self.nickname = "You";
        self.channelId = currentChannel;
        clients.append(self);
    }

    if (speaking) speakingClients.insert(clientId);
    else speakingClients.remove(clientId);

    if (isChannelMode()) {
        if (bubbles.contains(clientId) && bubbles[clientId]) {
            bubbles[clientId]->setNameStyle(speaking ? NewBubble::NameStyle::Speaking
                                                     : NewBubble::NameStyle::Idle);
        }
        return;
    }

    auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientInfo &c) { return c.id == clientId; });
    if (it == clients.end()) return;

    if (speaking) {
        showSpeakingClient(*it);
    } else {
        removeSpeakingClient(it->id);
    }
}

void WebSocketManager::handleClientMoved(const QJsonObject &payload)
{
    QString clientId = idToString(payload["clientId"]);
    QString newChannel = idToString(payload["newChannelId"]);
    QJsonObject props = payload["properties"].toObject();

    // Channel 0 = client left the server (or our view)
    if (newChannel == "0") {
        clients.erase(std::remove_if(clients.begin(), clients.end(),
                                     [&](const ClientInfo &c) { return c.id == clientId; }),
                      clients.end());
        speakingClients.remove(clientId);
        removeSpeakingClient(clientId);
        return;
    }

    auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientInfo &c) { return c.id == clientId; });
    if (it != clients.end()) {
        if (!newChannel.isEmpty()) it->channelId = newChannel;
        if (props.contains("nickname")) it->nickname = props["nickname"].toString();
    } else {
        if (props.isEmpty()) { LOG_WARNING("User props empty"); return; }

        ClientInfo info;
        info.id = clientId;
        info.channelId = newChannel;
        info.nickname = props["nickname"].toString();
        info.avatarUrl = props["myteamspeakAvatar"].toString().remove("2,");
        clients.append(info);
    }

    if (clientId == currentId && !newChannel.isEmpty()) {
        currentChannel = newChannel;
    }

    if (isChannelMode()) {
        rebuildClientBubbles();
    }
}

void WebSocketManager::handleClientPropertiesUpdated(const QJsonObject &payload)
{
    QString clientId = idToString(payload["clientId"]);
    QJsonObject props = payload["properties"].toObject();
    if (clientId.isEmpty() || !props.contains("nickname")) return;

    const QString nickname = props["nickname"].toString();
    auto it = std::find_if(clients.begin(), clients.end(), [&](const ClientInfo &c) { return c.id == clientId; });
    if (it != clients.end()) {
        if (it->nickname == nickname) return;
        it->nickname = nickname;
    } else {
        // Client we haven't seen yet (e.g. missing from the initial list)
        ClientInfo info;
        info.id = clientId;
        info.nickname = nickname;
        info.channelId = idToString(props["channelGroupInheritedChannelId"]);
        info.avatarUrl = props["myteamspeakAvatar"].toString().remove("2,");
        clients.append(info);
    }

    if (isChannelMode()) rebuildClientBubbles();
}

void WebSocketManager::handleTextMessage(const QJsonObject &payload)
{
    QString clientName = payload["invoker"].toObject()["nickname"].toString();
    QString msg = payload["message"].toString();

    LOG_DEBUG("Text message received from client: " + clientName + ", message: " + msg);

    if (!clientName.isEmpty()) {
        onUserMessageReceived(clientName, msg);
    } else {
        LOG_WARNING("Client not found for message: " + msg);
    }
}

void WebSocketManager::onConfigChanged(const QString &key, const QString &value)
{
    Q_UNUSED(value);
    if (key != "displayMode" && key != "overlayPosition") return;

    clearClientBubbles();
    if (isChannelMode()) {
        rebuildClientBubbles();
    } else {
        for (const ClientInfo &c : std::as_const(clients)) {
            if (speakingClients.contains(c.id)) showSpeakingClient(c);
        }
    }
}

void WebSocketManager::rebuildClientBubbles()
{
    if (!isChannelMode()) return;

    clearClientBubbles();
    if (currentChannel.isEmpty()) return;

    QList<ClientInfo> inChannel;
    for (const ClientInfo &c : std::as_const(clients)) {
        if (c.channelId == currentChannel) inChannel.append(c);
    }
    std::sort(inChannel.begin(), inChannel.end(), [](const ClientInfo &a, const ClientInfo &b) {
        return a.nickname.compare(b.nickname, Qt::CaseInsensitive) < 0;
    });

    const bool right = isRightAligned();
    for (const ClientInfo &c : std::as_const(inChannel)) {
        NewBubble *bubble = new NewBubble(c.nickname, defaultAvatar(), QString(), right, overlay);
        bubble->setNameStyle(speakingClients.contains(c.id) ? NewBubble::NameStyle::Speaking
                                                            : NewBubble::NameStyle::Idle);
        bubble->adjustSize();
        bubble->show();
        bubbles[c.id] = bubble;
    }
}

void WebSocketManager::clearClientBubbles()
{
    for (const QPointer<NewBubble> &bubble : std::as_const(bubbles)) {
        if (bubble) {
            bubble->hide();
            bubble->deleteLater();
        }
    }
    bubbles.clear();
}

void WebSocketManager::loadAvatarWithCache(const QString &avatarUrl, std::function<void(const QPixmap&)> callback)
{
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
        callback(defaultAvatar());
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
        const auto callbacks = pendingAvatarCallbacks.take(avatarUrl);

        QPixmap avatar;
        if (reply->error() != QNetworkReply::NoError) {
            LOG_ERROR("Failed to load avatar from: " + finalUrl + " Error: " + reply->errorString());
        } else if (!avatar.loadFromData(reply->readAll())) {
            LOG_ERROR("Failed to load avatar image data");
        } else {
            avatarCache[avatarUrl] = avatar;
            LOG_INFO("Avatar cached for URL: " + avatarUrl);
        }

        const QPixmap result = avatar.isNull() ? defaultAvatar() : avatar;
        for (const auto &cb : callbacks) {
            cb(result);
        }
    });
}

void WebSocketManager::showSpeakingClient(const ClientInfo &client)
{
    if (bubbles.contains(client.id)) return;

    LOG_DEBUG("Showing speaking bubble for: " + client.nickname); // TEMP-DEBUG
    NewBubble *bubble = new NewBubble(client.nickname, defaultAvatar(), QString(), isRightAligned(), overlay);
    bubble->adjustSize();
    bubble->show();
    bubbles[client.id] = bubble;
}

void WebSocketManager::onUserMessageReceived(const QString &clientName, const QString &message)
{
    LOG_INFO("Displaying message from: " + clientName + " | message: " + message);

    NewBubble *msgBubble = new NewBubble(clientName, defaultAvatar(), message, isRightAligned(), overlay);
    msgBubble->adjustSize();
    msgBubble->show();

    QTimer::singleShot(6000, msgBubble, [msgBubble]() {
        msgBubble->hide();
        msgBubble->deleteLater();
    });
}

void WebSocketManager::removeSpeakingClient(const QString &clientId)
{
    if (bubbles.contains(clientId)) {
        QPointer<NewBubble> bubble = bubbles.take(clientId);
        if (bubble) {
            bubble->hide();
            bubble->deleteLater();
        }
    }
}

void WebSocketManager::forceReconnect()
{
    LOG_INFO("Force reconnect called.");
    reconnectAttempts = 0;
    sentStoredApiKey = false;
    socket.abort();

    reconnectTimer.stop();
    connectToServer();
}
