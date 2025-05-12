#include "overlaywindow.h"
#include "./ui_overlaywindow.h"
#include "websocketmanager.h"

OverlayWindow::OverlayWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OverlayWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);

    WebSocketManager *wsManager = new WebSocketManager(this);
    wsManager->connectToServer();
}

OverlayWindow::~OverlayWindow()
{
    delete ui;
}
