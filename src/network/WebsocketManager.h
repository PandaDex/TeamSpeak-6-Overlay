#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QPointer>
#include <QSet>
#include <QHash>
#include <QMap>
#include <functional>
#include "../ui/NewBubble.h"

struct ClientInfo {
    QString id;
    QString nickname;
    QString avatarUrl;
    QString channelId;
};

class WebSocketManager : public QObject {
    Q_OBJECT

public:
    explicit WebSocketManager(QWidget *overlayParent, QObject *parent = nullptr);
    static WebSocketManager* instance();
    void connectToServer();
    void forceReconnect();

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onTextMessageReceived(QString message);
    void onConfigChanged(const QString &key, const QString &value);

private:
    void handleAuth(const QJsonObject &payload);
    void handleTalkStatusChanged(const QJsonObject &payload);
    void handleClientMoved(const QJsonObject &payload);
    void handleTextMessage(const QJsonObject &payload);
    void handleClientPropertiesUpdated(const QJsonObject &payload);

    void showSpeakingClient(const ClientInfo &client);
    void removeSpeakingClient(const QString &clientId);
    void onUserMessageReceived(const QString &clientName, const QString &message);

    // Channel list mode: rebuilds the list of clients in our current channel
    void rebuildClientBubbles();
    void clearClientBubbles();
    bool isChannelMode() const;
    bool isRightAligned() const;

    void scheduleReconnect();
    static QString idToString(const QJsonValue &value);
    QPixmap defaultAvatar();

    static WebSocketManager* m_instance;
    QWebSocket socket;
    QTimer reconnectTimer;
    QList<ClientInfo> clients;
    QSet<QString> speakingClients;
    QString currentId;
    QString currentChannel;
    QWidget *overlay;
    // Bubbles of clients (speaking clients, or whole channel in channel mode)
    QMap<QString, QPointer<NewBubble>> bubbles;
    QNetworkAccessManager networkManager;

    bool authenticated = false;
    bool sentStoredApiKey = false;

    // Correlates clientSelfPropertyUpdated(flagTalking) with talkStatusChanged
    // to find our own client id when TeamSpeak sends no auth reply
    int pendingSelfTalk = -1;
    QElapsedTimer pendingSelfTalkTimer;

    QPixmap m_defaultAvatar;
    QHash<QString, QPixmap> avatarCache;
    QSet<QString> pendingAvatarRequests;
    QHash<QString, QList<std::function<void(const QPixmap&)>>> pendingAvatarCallbacks;

    void loadAvatarWithCache(const QString &avatarUrl, std::function<void(const QPixmap&)> callback);

    int reconnectAttempts = 0;
};

#endif // WEBSOCKETMANAGER_H
