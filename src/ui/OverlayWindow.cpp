#include "OverlayWindow.h"
#include "./ui_OverlayWindow.h"
#include "../network/WebsocketManager.h"
#include "../core/ConfigManager.h"
#include "../core/OverlayPosition.h"

#ifdef Q_OS_LINUX
#include <QGuiApplication>
#include <QWindow>
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#endif

OverlayWindow::OverlayWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OverlayWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    auto updateAlignment = [this](const QString& position) {
        ui->verticalLayout->setAlignment(OverlayPosition::layoutAlignment(OverlayPosition::fromString(position)));
    };

    QObject::connect(ConfigManager::instance(), &ConfigManager::configChanged,
            this, [updateAlignment](const QString& key, const QString& value) {
                if (key == "overlayPosition") {
                    updateAlignment(value);
                }
            });

    updateAlignment(ConfigManager::get("overlayPosition"));

    // Initialize WebSocket
    WebSocketManager *wsManager = new WebSocketManager(this);
    wsManager->connectToServer();
}

void OverlayWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

#ifdef Q_OS_LINUX
    // Set X11 properties to make window click-through
    QWindow *window = windowHandle();
    if (window) {
        auto *native = qApp->nativeInterface<QNativeInterface::QX11Application>();
        if (native) {
            Display *display = native->display();
            if (display) {
                Window xWindow = static_cast<Window>(winId());
                XRectangle rect;
                rect.x = 0;
                rect.y = 0;
                rect.width = 0;
                rect.height = 0;
                XShapeCombineRectangles(display, xWindow, ShapeInput, 0, 0, &rect, 0, ShapeSet, 0);
            }
        }
    }
#endif
}

QWidget* OverlayWindow::getLayoutContainer()
{
    return ui->verticalLayoutWidget;
}

OverlayWindow::~OverlayWindow()
{
    delete ui;
}
