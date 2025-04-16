import { app, Tray, Menu, nativeImage } from 'electron';
import path from 'node:path';
import started from 'electron-squirrel-startup';
import AutoLaunch from 'auto-launch';
import { setupIpcHandlers } from './ipc/handlers';
import { getConfigPath, initializeConfig } from './config/config-manager';
import { getWindowPosition, createMainWindow, createSettingsWindow } from './windows/window-manager';

let mainWindow = null;
let settingsWindow = null;
let tray = null;
let _icon = null;

const CONFIG_PATH = getConfigPath();
let config = null;

if (started) {
  console.log('[MAIN] [INFO] > Squirrel startup detected, quitting app');
  app.quit();
}

function createTray(icon) {
  console.log('[MAIN] [INFO] > Creating tray icon');
  tray = new Tray(icon);

  const contextMenu = Menu.buildFromTemplate([
    {
      label: 'Show/Hide',
      click: () => mainWindow?.isVisible() ? mainWindow.hide() : mainWindow?.show(),
    },
    {
      label: 'Settings',
      click: () => {
        if (settingsWindow) {
          settingsWindow.focus();
          return;
        }
        settingsWindow = createSettingsWindow(_icon);
        settingsWindow.on('closed', () => {
          settingsWindow = null;
        });
      },
    },
    {
      label: 'Quit',
      click: () => app.quit(),
    },
  ]);

  tray.setContextMenu(contextMenu);
  tray.setToolTip('Teamspeak Overlay');
}

app.whenReady().then(() => {
  console.log('[MAIN] [INFO] > App is ready, initializing...');
  config = initializeConfig(CONFIG_PATH);
  setupIpcHandlers(config, CONFIG_PATH);

  const overlayLauncher = new AutoLaunch({
    name: 'Teamspeak Overlay'
  });

  if (config.startWithWindows) {
    console.log('[MAIN] [INFO] > Enabling auto-launch');
    if (!overlayLauncher.isEnabled()) overlayLauncher.enable();
  } else {
    console.log('[MAIN] [INFO] > Disabling auto-launch');
    if (overlayLauncher.isEnabled()) overlayLauncher.disable();
  }

  const iconPath = MAIN_WINDOW_VITE_DEV_SERVER_URL
    ? path.join(__dirname, 'icons/icon@1x.png')
    : path.join(__dirname, `../renderer/${MAIN_WINDOW_VITE_NAME}/icons/icon@1x.png`);

  _icon = nativeImage.createFromPath(iconPath);

  const position = getWindowPosition(config);
  console.log('[MAIN] [INFO] > Creating main window');
  mainWindow = createMainWindow(config, position, iconPath);
  createTray(_icon);
});

app.on('before-quit', () => {
  console.log('[MAIN] [INFO] > Application is quitting, cleaning up...');
  try {
    mainWindow?.destroy();
    settingsWindow?.destroy();
    tray?.destroy();
  } catch (error) {
    console.error('[MAIN] [ERROR] > Failed to destroy windows:', error);
  }
});