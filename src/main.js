import { app, BrowserWindow, Tray, Menu, nativeImage, ipcMain, screen } from 'electron';
import path from 'node:path';
import fs from 'node:fs';
import os from 'node:os';
import started from 'electron-squirrel-startup';
import AutoLaunch from 'auto-launch';

// Constants
const CONFIG_PATH = path.join(os.homedir(), 'AppData/Roaming/teamspeak-overlay/config.json');
const DEFAULT_CONFIG = {
  position: 'tl',
  padding: 10,
  startWithWindows: false,
  opacity: 100,
};

// Global variables
let mainWindow = null;
let settingsWindow = null;
let tray = null;
let config = DEFAULT_CONFIG;
let _icon = null;

// Exit if another instance is running
if (started) {
  app.quit();
}

// Load or create config file
function initializeConfig() {
  try {
    if (fs.existsSync(CONFIG_PATH)) {
      const data = JSON.parse(fs.readFileSync(CONFIG_PATH, 'utf8'));
      config = { ...DEFAULT_CONFIG, ...data };
    } else {
      fs.writeFileSync(CONFIG_PATH, JSON.stringify(DEFAULT_CONFIG), { encoding: 'utf-8' });
    }
  } catch (error) {
    console.error('Failed to initialize config:', error);
  }
}

// IPC handlers
ipcMain.on('get-config', (event) => {
  event.reply('config', config);
});

ipcMain.on('get-version', (event) => {
  event.reply('version', app.getVersion());
});

ipcMain.on('save-config', (_, newConfig) => {
  try {
    fs.writeFileSync(CONFIG_PATH, JSON.stringify(newConfig), { encoding: 'utf-8' });
    app.relaunch();
    app.exit();
  } catch (error) {
    console.error('Failed to save config:', error);
  }
});

const overlayLauncher = new AutoLaunch({
  name: 'Teamspeak Overlay'
});

if (config.startWithWindows) {
  if (!overlayLauncher.isEnabled()) overlayLauncher.enable();
} else {
  if (overlayLauncher.isEnabled()) overlayLauncher.disable();
}

function getWindowPosition() {
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

function createMainWindow() {
  const position = getWindowPosition();

  mainWindow = new BrowserWindow({
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

  const iconPath = MAIN_WINDOW_VITE_DEV_SERVER_URL
    ? path.join(__dirname, 'icons/icon@1x.png')
    : path.join(__dirname, `../renderer/${MAIN_WINDOW_VITE_NAME}/icons/icon@1x.png`);

  const icon = nativeImage.createFromPath(iconPath);

  if (MAIN_WINDOW_VITE_DEV_SERVER_URL) {
    mainWindow.loadURL(MAIN_WINDOW_VITE_DEV_SERVER_URL + position.queryParams);
  } else {
    mainWindow.loadFile(
      path.join(__dirname, `../renderer/${MAIN_WINDOW_VITE_NAME}/index.html`),
      { search: position.queryParams.replace('?', '') }
    );
    mainWindow.setIcon(iconPath);
  }
  _icon = icon;
  createTray(icon);
}

function createSettingsWindow() {
  if (settingsWindow) {
    settingsWindow.focus();
    return;
  }

  settingsWindow = new BrowserWindow({
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
    settingsWindow.loadURL(`${MAIN_WINDOW_VITE_DEV_SERVER_URL}/#/settings`);
  } else {
    settingsWindow.loadFile(
      path.join(__dirname, `../renderer/${MAIN_WINDOW_VITE_NAME}/index.html`),
      { hash: '/settings' }
    );
  }

  settingsWindow.setIcon(_icon);

  settingsWindow.on('closed', () => {
    settingsWindow = null;
  });
}

function createTray(icon) {
  tray = new Tray(icon);

  const contextMenu = Menu.buildFromTemplate([
    {
      label: 'Show/Hide',
      click: () => mainWindow?.isVisible() ? mainWindow.hide() : mainWindow?.show(),
    },
    {
      label: 'Settings',
      click: createSettingsWindow,
    },
    {
      label: 'Quit',
      click: () => app.quit(),
    },
  ]);

  tray.setContextMenu(contextMenu);
  tray.setToolTip('Teamspeak Overlay');
}

// App event handlers
app.whenReady().then(() => {
  initializeConfig();
  createMainWindow();
});

app.on('before-quit', () => {
  try {
    mainWindow?.destroy();
    settingsWindow?.destroy();
    tray?.destroy();
  } catch (error) {
    console.error('Failed to destroy windows:', error);
  }
});