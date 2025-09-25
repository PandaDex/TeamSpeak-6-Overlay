#include "MessageBubble.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "../core/DatabaseManager.h"

MessageBubble::MessageBubble(const QString &username, const QString &message, QWidget *parent)
    : QWidget(parent)
{

    QString fontSize = DatabaseManager::get("fontSize");


    messageLabel = new QLabel(username+": "+message, this);
    messageLabel->setStyleSheet("color: white; font-size: " + fontSize + "px; min-width:200px; max-width: 500px; padding: 4px 6px; background-color: rgba(0, 0, 0, 120); border-radius: 6px");
    messageLabel->setWordWrap(true);

    // Layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->addWidget(messageLabel);
    setLayout(layout);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    if (parent) {
        QVBoxLayout *parentLayout = parent->findChild<QVBoxLayout*>();
        if (parentLayout) {
            parentLayout->addWidget(this);
        }
    }
}
