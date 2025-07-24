#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

#ifndef MESSAGEBUBBLE_H
#define MESSAGEBUBBLE_H



class MessageBubble : public QWidget {

public:
    explicit MessageBubble(const QString &username, const QString &message, QWidget *parent = nullptr);

private:
    QLabel *nameLabel;
};



#endif //MESSAGEBUBBLE_H

