#ifndef USERBUBBLE_H
#define USERBUBBLE_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class UserBubble : public QWidget
{
    Q_OBJECT

public:
    explicit UserBubble(const QString &username, const QPixmap &avatar, QWidget *parent = nullptr);

private:
    QLabel *avatarLabel;
    QLabel *nameLabel;
};

#endif // USERBUBBLE_H
