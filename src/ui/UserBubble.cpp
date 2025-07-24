#include "UserBubble.h"
#include <QPainter>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QLabel>
#include "../core/Constants.h"

UserBubble::UserBubble(const QString &username, const QPixmap &avatar, QWidget *parent)
    : QWidget(parent)
{
    // Create a rounded avatar pixmap
    QPixmap scaledAvatar = avatar.scaled(Constants::DEFAULT_AVATAR_SIZE, Constants::DEFAULT_AVATAR_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPixmap roundedAvatar(Constants::DEFAULT_AVATAR_SIZE, Constants::DEFAULT_AVATAR_SIZE);
    roundedAvatar.fill(Qt::transparent);

    QPainter painter(&roundedAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addEllipse(0, 0, Constants::DEFAULT_AVATAR_SIZE, Constants::DEFAULT_AVATAR_SIZE);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, scaledAvatar);

    // Avatar label
    avatarLabel = new QLabel(this);
    avatarLabel->setPixmap(roundedAvatar);
    avatarLabel->setFixedSize(Constants::DEFAULT_AVATAR_SIZE, Constants::DEFAULT_AVATAR_SIZE);
    avatarLabel->setStyleSheet("background: transparent;");

    // Username label
    nameLabel = new QLabel(username, this);
    nameLabel->setStyleSheet("color: white; font-weight: bold; padding-left: 6px; padding-right: 6px; background-color: rgba(0, 0, 0, 120); border-radius: 6px");

    // Layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(4);
    layout->addWidget(avatarLabel);
    layout->addWidget(nameLabel);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setLayout(layout);

    if (parent) {
        QVBoxLayout *parentLayout = parent->findChild<QVBoxLayout*>();
        if (parentLayout) {
            parentLayout->addWidget(this);
        }
    }
}
