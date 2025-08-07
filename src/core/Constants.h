#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants {
    constexpr int DEFAULT_AVATAR_SIZE = 20;
    constexpr int DEFAULT_PADDING = 10;
    constexpr int DEFAULT_OPACITY = 100;
    constexpr int BUBBLE_VERTICAL_SPACING = 30;
    constexpr int BUBBLE_MARGIN = 10;

    constexpr int MAX_RECONNECT_DELAY_MS = 30000;
    constexpr int BASE_RECONNECT_DELAY_MS = 1000;
    const QString WEBSOCKET_URL = "ws://localhost:5899/";

    const QString APP_IDENTIFIER = "dex.teamspeak6.overlay";
    const QString APP_VERSION = "1.0.3";
    const QString APP_NAME = "Teamspeak Overlay";
    const QString APP_DESCRIPTION = "Shows speaking clients on screen";

    const QString DB_NAME = "config.db";

    const QString DEFAULT_AVATAR_URL = "https://raw.githubusercontent.com/PandaDex/TeamSpeak-6-Overlay/refs/heads/master/resources/icon%401x.ico";
}

#endif // CONSTANTS_H