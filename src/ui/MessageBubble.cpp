#include "MessageBubble.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

MessageBubble::MessageBubble(const QString &username, const QString &message, QWidget *parent)
    : QWidget(parent)
{

    QString trimmed;
    if (message.length() > 11) {
        trimmed = message;
        trimmed.chop(3);
        trimmed += "...";
    }


    // Username label
    nameLabel = new QLabel(username+": "+trimmed, this);
    nameLabel->setStyleSheet("color: white; font-weight: bold; padding-left: 6px; padding-right: 6px; background-color: rgba(0, 0, 0, 120); border-radius: 6px");

    // Layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->addWidget(nameLabel);
    setLayout(layout);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    if (parent) {
        QVBoxLayout *parentLayout = parent->findChild<QVBoxLayout*>();
        if (parentLayout) {
            parentLayout->addWidget(this);
        }
    }
}
