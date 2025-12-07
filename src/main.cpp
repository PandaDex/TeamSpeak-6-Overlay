#include "ui/OverlayWindow.h"
#include "core/DatabaseManager.h"
#include "ui/SettingsWindow.h"
#include "core/Constants.h"
#include "core/Logger.h"
#include "network/WebsocketManager.h"
#include "singleapplication.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
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
    // Initialize database
    DatabaseManager::initialize();

    // Load translations
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TeamSpeak-Overlay_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

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

    if(DatabaseManager::get("fontSize").isEmpty()) {
        DatabaseManager::set("fontSize", "12");
    }

    if(!DatabaseManager::get("overlayPosition").isEmpty()){
        VAR_CONFIG_POSITION = DatabaseManager::get("overlayPosition").toInt();
    }

    if(!DatabaseManager::get("overlayPadding").isEmpty()){
        VAR_CONFIG_PADDING = DatabaseManager::get("overlayPadding").toInt();
    }

    if(!DatabaseManager::get("overlayOpacity").isEmpty()){
        VAR_CONFIG_OPACITY = DatabaseManager::get("overlayOpacity").toInt();
    }

    OverlayWindow w;
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    int windowWidth = w.width();
    int windowHeight = w.height();

    if (VAR_CONFIG_POSITION == 1) {
        w.move(screenGeometry.right() - windowWidth - VAR_CONFIG_PADDING,
               screenGeometry.top() + VAR_CONFIG_PADDING);
    } else if (VAR_CONFIG_POSITION == 2) {
        w.move(screenGeometry.left() + VAR_CONFIG_PADDING,
               screenGeometry.bottom() - windowHeight - VAR_CONFIG_PADDING);
    } else if (VAR_CONFIG_POSITION == 3) {
        w.move(screenGeometry.right() - windowWidth - VAR_CONFIG_PADDING,
               screenGeometry.bottom() - windowHeight - VAR_CONFIG_PADDING);
    } else {
        w.move(screenGeometry.left() + VAR_CONFIG_PADDING,
               screenGeometry.top() + VAR_CONFIG_PADDING);
    }

    LOG_DEBUG("==CONFIG==");
    LOG_DEBUG(QString::number(VAR_CONFIG_POSITION));
    LOG_DEBUG(QString::number(VAR_CONFIG_PADDING));
    LOG_DEBUG(QString::number(VAR_CONFIG_OPACITY));
    LOG_DEBUG("==========");

    w.show();

    // This ensures opacity works properly on Linux X11
    QTimer::singleShot(0, [&w, VAR_CONFIG_OPACITY]() {
        w.setWindowOpacity(VAR_CONFIG_OPACITY / 100.0);
    });

    return a.exec();
}
