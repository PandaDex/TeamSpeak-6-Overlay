# TeamSpeak 6 Overlay

[![Release](https://img.shields.io/github/v/release/PandaDex/TeamSpeak-6-Overlay)](https://github.com/PandaDex/TeamSpeak-6-Overlay/releases)
[![Downloads](https://img.shields.io/github/downloads/PandaDex/TeamSpeak-6-Overlay/total)](https://github.com/PandaDex/TeamSpeak-6-Overlay/releases)
[![Issues](https://img.shields.io/github/issues/PandaDex/TeamSpeak-6-Overlay)](https://github.com/PandaDex/TeamSpeak-6-Overlay/issues)
[![License](https://img.shields.io/github/license/PandaDex/TeamSpeak-6-Overlay)](LICENSE)

A lightweight, performance-focused in-game overlay for TeamSpeak 5 and 6 that displays speaking users from your current channel on screen.

<img src="https://raw.githubusercontent.com/PandaDex/TeamSpeak-6-Overlay/refs/heads/master/markdown/game_720.png" height="300" />

## Quick Start

### Installation

1. **Download** the latest version from the [releases page](https://github.com/PandaDex/TeamSpeak-6-Overlay/releases)
2. **Run** the installer
3. **Accept** the connection request in your TeamSpeak client when prompted
4. **Done!** The overlay will automatically appear when users speak

## Development Status

This project is currently in active development. The Qt6 rewrite(currently master) focuses on performance and maintainability improvements over the previous Electron version.

### Current Roadmap

#### Priority Features (Required for Stable Release)

- [x] **Polish WebSocket code** - Improve connection stability
- [x] **Enhanced error handling** - Reduce crashes and improve reliability
- [x] **Code optimization** - Performance improvements and cleanup
- [x] **NSIS Installer** - Simple windows installer
- [x] **Feature parity** - Match all Electron version capabilities

#### Future Enhancements

- [x] **Improved configuration storage** - Better settings management
- [x] **Bubble redesign** - Redesign user and message bubbles, make it more unified.
- [ ] **Appearance customization** - Additional setting: font, font size, background & text colors, toggle avatars, toggle messages.
- [ ] **Avatar fix** - Finally fix avatar logic to use clients avatars instead of placeholder
## Issues & Support

Found a bug or have a feature request? Please [create an issue](https://github.com/PandaDex/TeamSpeak-6-Overlay/issues) with:

- Your operating system
- TeamSpeak client version
- Steps to reproduce the issue
- Screenshots if applicable

## Contributing

Contributions are welcome! Whether it's:

- Bug reports and fixes
- Feature suggestions and implementations
- Documentation improvements
- Translations

**Before contributing code**, please note that all contributors must sign a Contributor License Agreement (CLA) to ensure proper licensing and legal clarity for the project.

Please feel free to open an issue or submit a pull request.

## License

This project is licensed under the [GPL-3.0 License](LICENSE) - see the LICENSE file for details.


