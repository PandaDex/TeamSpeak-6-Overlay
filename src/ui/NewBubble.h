#ifndef NEWBUBBLE_H
#define NEWBUBBLE_H

#include <QWidget>
#include <QLabel>

class NewBubble : public QWidget
{
    Q_OBJECT

public:
    explicit NewBubble(const QString &username, const QPixmap &avatar,
                       const QString &message, const bool &b_rightSide, QWidget *parent = nullptr);

private:
    QLabel *avatarLabel;
    QLabel *nameLabel;
    QLabel *messageLabel;
};

#endif // NEWBUBBLE_H
