#include "userbubble.h"

UserBubble::UserBubble(const QString &username, const QPixmap &avatar, QWidget *parent)
    : QWidget(parent)
{
    // Avatar
    avatarLabel = new QLabel(this);
    avatarLabel->setPixmap(avatar.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Username
    nameLabel = new QLabel(username, this);
    nameLabel->setStyleSheet("color: white; font-weight: bold;");

    // Layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 2, 5, 2);
    layout->addWidget(avatarLabel);
    layout->addWidget(nameLabel);

    setLayout(layout);
    setStyleSheet("background-color: rgba(0, 0, 0, 120); border-radius: 6px;");
}
