#include "ui/OverlayWindow.h"
#include "core/ConfigManager.h"
#include "ui/SettingsWindow.h"
#include "core/Constants.h"
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
    Logger::setLogLevel(Logger::Level::Warning);
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    SingleApplication single(argc, argv);
    ConfigManager::initialize();

    // Create the tray icon
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon;
    trayIcon->setIcon(QIcon(":/resources/icon@3x.png"));

    QMenu *trayMenu = new QMenu;

    QAction *reconnectAction = new QAction("Force Reconnect", trayMenu);
    QObject::connect(reconnectAction, &QAction::triggered, [&]() {
        WebSocketManager::instance()->forceReconnect();
    });

    QAction *settingsAction = new QAction("Settings", trayMenu);
    QObject::connect(settingsAction, &QAction::triggered, [&]() {
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

    int VAR_CONFIG_POSITION = 0;
    int VAR_CONFIG_PADDING = Constants::DEFAULT_PADDING;
    int VAR_CONFIG_OPACITY = Constants::DEFAULT_OPACITY;

    QString overlayPosition = ConfigManager::get("overlayPosition");
    overlayPosition.isEmpty() ? VAR_CONFIG_POSITION = 0 : VAR_CONFIG_POSITION = overlayPosition.toInt();
    QString overlayPadding = ConfigManager::get("overlayPadding");
    overlayPadding.isEmpty() ? VAR_CONFIG_PADDING = Constants::DEFAULT_PADDING : VAR_CONFIG_PADDING = overlayPadding.toInt();
    QString overlayOpacity = ConfigManager::get("overlayOpacity");
    overlayOpacity.isEmpty() ? VAR_CONFIG_OPACITY = Constants::DEFAULT_OPACITY : VAR_CONFIG_OPACITY = overlayOpacity.toInt();

    OverlayWindow w;
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    int windowWidth = w.width();
    int windowHeight = w.height();

    auto updateWindowPosition = [&w, &screenGeometry, &windowWidth, &windowHeight](int position, int padding) {
        if (position == 1) {
            w.move(screenGeometry.right() - windowWidth - padding,
                   screenGeometry.top() + padding);
        } else if (position == 2) {
            w.move(screenGeometry.left() + padding,
                   screenGeometry.bottom() - windowHeight - padding);
        } else if (position == 3) {
            w.move(screenGeometry.right() - windowWidth - padding,
                   screenGeometry.bottom() - windowHeight - padding);
        } else {
            w.move(screenGeometry.left() + padding,
                   screenGeometry.top() + padding);
        }
    };

    updateWindowPosition(VAR_CONFIG_POSITION, VAR_CONFIG_PADDING);

    QObject::connect(ConfigManager::instance(), &ConfigManager::configChanged,
            &w, [&w, &VAR_CONFIG_POSITION, &VAR_CONFIG_PADDING, &VAR_CONFIG_OPACITY, updateWindowPosition](const QString& key, const QString& value) {
                if (key == "overlayPosition") {
                    VAR_CONFIG_POSITION = value.toInt();
                    updateWindowPosition(VAR_CONFIG_POSITION, VAR_CONFIG_PADDING);
                } else if (key == "overlayPadding") {
                    VAR_CONFIG_PADDING = value.toInt();
                    updateWindowPosition(VAR_CONFIG_POSITION, VAR_CONFIG_PADDING);
                } else if (key == "overlayOpacity") {
                    VAR_CONFIG_OPACITY = value.toInt();
                    w.setWindowOpacity(VAR_CONFIG_OPACITY / 100.0);
                }
            });

    w.show();

    // This ensures opacity works properly on Linux X11
    QTimer::singleShot(0, [&w, VAR_CONFIG_OPACITY]() {
        w.setWindowOpacity(VAR_CONFIG_OPACITY / 100.0);
    });

    return a.exec();
}
