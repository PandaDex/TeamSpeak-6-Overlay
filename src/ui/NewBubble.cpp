#include "NewBubble.h"
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "../core/Constants.h"

NewBubble::NewBubble(const QString &username, const QPixmap &avatar, const QString &message, const bool &b_rightSide, QWidget *parent)
    : QWidget{parent}
{
    QPixmap scaledAvatar = avatar.scaled(Constants::DEFAULT_AVATAR_SIZE, Constants::DEFAULT_AVATAR_SIZE,
                                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPixmap roundedAvatar(Constants::DEFAULT_AVATAR_SIZE, Constants::DEFAULT_AVATAR_SIZE);
    roundedAvatar.fill(Qt::transparent);
    QPainter painter(&roundedAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addEllipse(0, 0, Constants::DEFAULT_AVATAR_SIZE, Constants::DEFAULT_AVATAR_SIZE);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, scaledAvatar);

    avatarLabel = new QLabel(this);
    avatarLabel->setPixmap(roundedAvatar);
    avatarLabel->setFixedSize(Constants::DEFAULT_AVATAR_SIZE, Constants::DEFAULT_AVATAR_SIZE);
    avatarLabel->setStyleSheet("background: transparent;");
    avatarLabel->setAlignment(Qt::AlignTop);

    nameLabel = new QLabel(username, this);
    setNameStyle(NameStyle::Normal);
    nameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    if (!message.isEmpty()) {
        messageLabel = new QLabel(message, this);
        messageLabel->setStyleSheet(
            "color: white; "
            "padding: 6px 10px; "
            "background-color: rgba(0, 0, 0, 102); "
            "border-radius: 6px;"
        );
        messageLabel->setWordWrap(true);
        messageLabel->setMaximumWidth(450);
        messageLabel->setMaximumHeight(300);
        messageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        messageLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    }

    // Vertical layout for username and message
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(4);
    if (b_rightSide) {
        textLayout->addWidget(nameLabel, 0, Qt::AlignRight);
    } else {
        textLayout->addWidget(nameLabel, 0, Qt::AlignLeft);
    }
    if (!message.isEmpty()) {
        textLayout->addWidget(messageLabel);
    }

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(8);

    // Layout based on b_rightSide
    if (b_rightSide) {
        // Right side: stretch, text, avatar
        mainLayout->addStretch();
        mainLayout->addLayout(textLayout);
        mainLayout->addWidget(avatarLabel, 0, Qt::AlignTop);

        // Align username to the right
        nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if (!message.isEmpty()) {
            messageLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);
        }
    } else {
        // Left side: avatar, text, stretch
        mainLayout->addWidget(avatarLabel, 0, Qt::AlignTop);
        mainLayout->addLayout(textLayout);
        mainLayout->addStretch();

        // Align username to the left
        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        if (!message.isEmpty()) {
            messageLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        }
    }

    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    if (parent) {
        QVBoxLayout *parentLayout = parent->findChild<QVBoxLayout*>();
        if (parentLayout) {
            parentLayout->addWidget(this);
        }
    }
}

void NewBubble::setNameStyle(NameStyle style)
{
    // All variants keep the same total box size (border + padding) so the
    // bubble doesn't jump when the speaking state toggles.
    switch (style) {
    case NameStyle::Normal:
        nameLabel->setStyleSheet(
            "color: white; "
            "font-weight: bold; "
            "padding: 4px 8px; "
            "background-color: rgba(0, 0, 0, 102); "  // 40% opacity (255 * 0.4 = 102)
            "border-radius: 6px;"
        );
        break;
    case NameStyle::Idle:
        nameLabel->setStyleSheet(
            "color: rgba(255, 255, 255, 160); "
            "font-weight: bold; "
            "padding: 2px 6px; "
            "border: 2px solid transparent; "
            "background-color: rgba(0, 0, 0, 80); "
            "border-radius: 6px;"
        );
        break;
    case NameStyle::Speaking:
        nameLabel->setStyleSheet(
            "color: white; "
            "font-weight: bold; "
            "padding: 2px 6px; "
            "border: 2px solid rgb(46, 204, 113); "
            "background-color: rgba(0, 0, 0, 102); "
            "border-radius: 6px;"
        );
        break;
    }
    // A disabled QLabel renders its pixmap greyed out, dimming silent clients
    avatarLabel->setEnabled(style != NameStyle::Idle);
}
