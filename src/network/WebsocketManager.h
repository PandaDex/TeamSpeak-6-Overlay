#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../ui/UserBubble.h"
#include "../ui/MessageBubble.h"

struct ClientInfo {
    QString id;
    QString nickname;
    QString avatarUrl;
};

class WebSocketManager : public QObject {
    Q_OBJECT

public:
    explicit WebSocketManager(QWidget *overlayParent, QObject *parent = nullptr);
    static WebSocketManager* instance();
    void connectToServer();
    bool ALG_RIGHT{};
    bool ALG_BOTTOM{};
    void forceReconnect();

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onTextMessageReceived(QString message);

private:
    void showSpeakingClient(const ClientInfo &client);
    void onUserMessageReceived(const QString clientName, QString message,int id);
    void removeSpeakingClient(const QString &clientId);
    void scheduleReconnect();

    QMap<QString, MessageBubble*> messageBubbles;
    static WebSocketManager* m_instance;
    QWebSocket socket;
    QList<ClientInfo> clients;
    QString currentId;
    int currentChannel = -1;
    QWidget *overlay;
    QMap<QString, UserBubble*> bubbles;
    QNetworkAccessManager networkManager;

    int reconnectAttempts = 0;
};

#endif // WEBSOCKETMANAGER_H
