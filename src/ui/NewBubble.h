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

    enum class NameStyle {
        Normal,   // default look (speaking-only mode, messages)
        Idle,     // channel list mode, client is silent
        Speaking  // channel list mode, client is talking
    };
    void setNameStyle(NameStyle style);

private:
    QLabel *avatarLabel = nullptr;
    QLabel *nameLabel = nullptr;
    QLabel *messageLabel = nullptr;
};

#endif // NEWBUBBLE_H
