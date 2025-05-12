#include "overlaywindow.h"

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

    // Create context menu for tray
    QMenu *trayMenu = new QMenu;

    QAction *settingsAction = new QAction("Settings", trayMenu);
    settingsAction->setDisabled(true);

    QAction *quitAction = new QAction("Quit", trayMenu);
    QObject::connect(quitAction, &QAction::triggered, &a, &QApplication::quit);

    trayMenu->addAction(settingsAction);
    trayMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    OverlayWindow w;
    w.move(10, 10);
    w.show();

    return a.exec();
}
