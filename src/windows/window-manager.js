import { BrowserWindow, screen } from 'electron';
import path from 'node:path';

export function getWindowPosition(config) {
    console.log('[WINDOW_MANAGER] [INFO] > Calculating window position');
    const { workAreaSize } = screen.getPrimaryDisplay();
    const position = {
        x: 0,
        y: 0,
        queryParams: '',
    };

    switch (config.position) {
        case 'tr':
            position.x = workAreaSize.width - 170 - config.padding;
            position.y = config.padding;
            position.queryParams = '?x=true';
            break;
        case 'bl':
            position.x = config.padding;
            position.y = workAreaSize.height - 250 - config.padding;
            position.queryParams = '?y=true';
            break;
        case 'br':
            position.x = workAreaSize.width - 170 - config.padding;
            position.y = workAreaSize.height - 250 - config.padding;
            position.queryParams = '?x=true&y=true';
            break;
        default: // tl
            position.x = config.padding;
            position.y = config.padding;
    }

    return position;
}

export function createMainWindow(config, position, icon) {
    console.log('[WINDOW_MANAGER] [INFO] > Creating main window');
    const mainWindow = new BrowserWindow({
        width: 170,
        height: 250,
        skipTaskbar: true,
        transparent: true,
        frame: false,
        alwaysOnTop: true,
        x: position.x,
        y: position.y,
        resizable: false,
        opacity: config.opacity / 100,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false,
        },
    });

    mainWindow.setVisibleOnAllWorkspaces(true);
    mainWindow.setIgnoreMouseEvents(true);

    if (MAIN_WINDOW_VITE_DEV_SERVER_URL) {
        console.log('[WINDOW_MANAGER] [INFO] > Loading dev server URL');
        mainWindow.loadURL(MAIN_WINDOW_VITE_DEV_SERVER_URL + position.queryParams);
    } else {
        console.log('[WINDOW_MANAGER] [INFO] > Loading production build');
        mainWindow.loadFile(
            path.join(__dirname, `../../renderer/${MAIN_WINDOW_VITE_NAME}/index.html`),
            { search: position.queryParams.replace('?', '') }
        );
        mainWindow.setIcon(icon);
    }

    return mainWindow;
}

export function createSettingsWindow(icon) {
    console.log('[WINDOW_MANAGER] [INFO] > Creating settings window');
    const settingsWindow = new BrowserWindow({
        width: 1280,
        height: 720,
        frame: true,
        resizable: false,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false,
        },
    });

    settingsWindow.setMenuBarVisibility(false);
    if (MAIN_WINDOW_VITE_DEV_SERVER_URL) {
        console.log('[WINDOW_MANAGER] [INFO] > Loading settings dev server URL');
        settingsWindow.loadURL(`${MAIN_WINDOW_VITE_DEV_SERVER_URL}/#/settings`);
    } else {
        console.log('[WINDOW_MANAGER] [INFO] > Loading settings production build');
        settingsWindow.loadFile(
            path.join(__dirname, `../../renderer/${MAIN_WINDOW_VITE_NAME}/index.html`),
            { hash: '/settings' }
        );
    }

    settingsWindow.setIcon(icon);
    return settingsWindow;
}