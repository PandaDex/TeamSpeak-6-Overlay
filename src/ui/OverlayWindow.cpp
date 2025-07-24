#include "OverlayWindow.h"
#include "./ui_OverlayWindow.h"
#include "../network/WebsocketManager.h"
#include "../core/DatabaseManager.h"

OverlayWindow::OverlayWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OverlayWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    if(!DatabaseManager::get("overlayPosition").isEmpty()) {
        QString position = DatabaseManager::get("overlayPosition");
        if (position == "2") {
            ui->verticalLayout->setAlignment(Qt::AlignBottom);
        } else if (position == "3") {
            ui->verticalLayout->setAlignment(Qt::AlignBottom);
        } else {
            ui->verticalLayout->setAlignment(Qt::AlignTop);
        }
    } else {
         ui->verticalLayout->setAlignment(Qt::AlignTop);
    }

    WebSocketManager *wsManager = new WebSocketManager(this);
    wsManager->connectToServer();
}

QWidget* OverlayWindow::getLayoutContainer()  {
    return ui->verticalLayoutWidget;
}

OverlayWindow::~OverlayWindow()
{
    delete ui;
}
