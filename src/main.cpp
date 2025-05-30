#include "overlaywindow.h"
#include "databasemanager.h"
#include "settingswindow.h"
#include "websocketmanager.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QScreen>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
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

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning("System tray not available!");
        return 1;
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
    int VAR_CONFIG_PADDING = 10;
    int VAR_CONFIG_OPACITY = 100;

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

    w.setWindowOpacity(VAR_CONFIG_OPACITY/100.0);

    qDebug() << "==CONFIG==";
    qDebug() << VAR_CONFIG_POSITION;
    qDebug() << VAR_CONFIG_PADDING;
    qDebug() << VAR_CONFIG_OPACITY;
    qDebug() << "==========";

    w.show();

    return a.exec();
}

