#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants {
    constexpr int DEFAULT_AVATAR_SIZE = 32;
    constexpr int DEFAULT_PADDING = 10;
    constexpr int DEFAULT_OPACITY = 100;

    constexpr int MAX_RECONNECT_DELAY_MS = 30000;
    constexpr int BASE_RECONNECT_DELAY_MS = 1000;
    const QString WEBSOCKET_URL = "ws://localhost:5899/";

    const QString APP_IDENTIFIER = "dex.teamspeak6.overlay";
    const QString APP_VERSION = "1.0.5";
    const QString APP_NAME = "Teamspeak Overlay";
    const QString APP_DESCRIPTION = "Shows speaking clients on screen";

    const QString CONFIG_FILE_NAME = "config.cfg";
    const QString CONFIG_DIR_NAME = "TeamSpeak-Overlay";

    const QString DEFAULT_AVATAR_RESOURCE = ":/resources/icon@3x.png";

    const QString DEFAULT_AVATAR_URL = "https://raw.githubusercontent.com/PandaDex/TeamSpeak-6-Overlay/refs/heads/master/resources/icon%403x.png";
}

#endif // CONSTANTS_H
