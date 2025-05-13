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
#include "userbubble.h"

struct ClientInfo {
    QString id;
    QString nickname;
    QString avatarUrl;
};

class WebSocketManager : public QObject {
    Q_OBJECT

public:
    explicit WebSocketManager(QWidget *overlayParent, QObject *parent = nullptr);
    void connectToServer();

private slots:
    void onConnected();
    void onTextMessageReceived(QString message);

private:
    QWebSocket socket;
    QList<ClientInfo> clients;
    QString currentId;
    int currentChannel = -1;
    QWidget *overlay;
    QMap<QString, UserBubble*> bubbles;
    QNetworkAccessManager networkManager;

    void showSpeakingClient(const ClientInfo &client);
    void removeSpeakingClient(const QString &clientId);
};

#endif
