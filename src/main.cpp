#include "ui/OverlayWindow.h"
#include "core/ConfigManager.h"
#include "ui/SettingsWindow.h"
#include "core/Constants.h"
#include "core/OverlayPosition.h"
#include "core/Logger.h"
#include "network/WebsocketManager.h"
#include "singleapplication.h"
#include <QApplication>
#include <QScreen>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QTimer>

int main(int argc, char *argv[])
{
    Logger::setLogLevel(Logger::Level::Debug);
    SingleApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    ConfigManager::initialize();

    // Create the tray icon
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon;
    trayIcon->setIcon(QIcon(":/resources/icon@3x.png"));

    QMenu *trayMenu = new QMenu;

    QAction *reconnectAction = new QAction("Force Reconnect", trayMenu);
    QObject::connect(reconnectAction, &QAction::triggered, []() {
        WebSocketManager::instance()->forceReconnect();
    });

    QAction *settingsAction = new QAction("Settings", trayMenu);
    QObject::connect(settingsAction, &QAction::triggered, []() {
        SettingsWindow *settings = new SettingsWindow;
        settings->setAttribute(Qt::WA_DeleteOnClose);
        settings->show();
    });
    QAction *quitAction = new QAction("Quit", trayMenu);
    QObject::connect(quitAction, &QAction::triggered, &a, &QApplication::quit);

    trayMenu->addAction(reconnectAction);
    trayMenu->addAction(settingsAction);
    trayMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    auto readInt = [](const QString &key, int fallback) {
        bool ok = false;
        int v = ConfigManager::get(key).toInt(&ok);
        return ok ? v : fallback;
    };

    OverlayWindow w;

    auto updateWindowPosition = [&w, readInt]() {
        const QRect screen = QApplication::primaryScreen()->availableGeometry();
        const int position = OverlayPosition::fromString(ConfigManager::get("overlayPosition"));
        const int padding = readInt("overlayPadding", Constants::DEFAULT_PADDING);
        const int width = w.width();
        const int height = w.height();

        const int left = screen.left() + padding;
        const int right = screen.left() + screen.width() - width - padding;
        const int top = screen.top() + padding;
        const int bottom = screen.top() + screen.height() - height - padding;
        const int vcenter = screen.top() + (screen.height() - height) / 2;

        switch (position) {
        case OverlayPosition::TopRight:    w.move(right, top); break;
        case OverlayPosition::BottomLeft:  w.move(left, bottom); break;
        case OverlayPosition::BottomRight: w.move(right, bottom); break;
        case OverlayPosition::CenterLeft:  w.move(left, vcenter); break;
        case OverlayPosition::CenterRight: w.move(right, vcenter); break;
        case OverlayPosition::Custom:
            // Custom coordinates are relative to the primary screen's top-left corner
            w.move(screen.left() + readInt("overlayCustomX", 0),
                   screen.top() + readInt("overlayCustomY", 0));
            break;
        case OverlayPosition::TopLeft:
        default:                           w.move(left, top); break;
        }
    };

    updateWindowPosition();

    QObject::connect(ConfigManager::instance(), &ConfigManager::configChanged,
            &w, [&w, updateWindowPosition](const QString& key, const QString& value) {
                if (key == "overlayPosition" || key == "overlayPadding"
                    || key == "overlayCustomX" || key == "overlayCustomY") {
                    updateWindowPosition();
                } else if (key == "overlayOpacity") {
                    w.setWindowOpacity(value.toInt() / 100.0);
                }
            });

    w.show();

    // This ensures opacity works properly on Linux X11
    const int opacity = readInt("overlayOpacity", Constants::DEFAULT_OPACITY);
    QTimer::singleShot(0, &w, [&w, opacity]() {
        w.setWindowOpacity(opacity / 100.0);
    });

    return a.exec();
}
