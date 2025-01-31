import { app, BrowserWindow, Tray, Menu, nativeImage } from 'electron';
import path from 'node:path';
import started from 'electron-squirrel-startup';

if (started) {
  app.quit();
}

let mainWindow;
let tray;

const createWindow = () => {
  mainWindow = new BrowserWindow({
    width: 170,
    height: 250,
    skipTaskbar: true,
    transparent: true,
    frame: false,
    alwaysOnTop: true,
    x: 10,
    y: 10,
    resizable: false,
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
    },
  });


  mainWindow.setVisibleOnAllWorkspaces(true);
  mainWindow.setIgnoreMouseEvents(true);
  let icon = nativeImage.createFromPath(path.join(__dirname, `icon.png`));

  if (MAIN_WINDOW_VITE_DEV_SERVER_URL) {
    mainWindow.loadURL(MAIN_WINDOW_VITE_DEV_SERVER_URL);
  } else {
    mainWindow.loadFile(path.join(__dirname, `../renderer/${MAIN_WINDOW_VITE_NAME}/index.html`));
    mainWindow.setIcon(path.join(__dirname, `../renderer/${MAIN_WINDOW_VITE_NAME}/icon.png`));
    icon = nativeImage.createFromPath(path.join(__dirname, `../renderer/${MAIN_WINDOW_VITE_NAME}/icon.png`));
  }

  tray = new Tray(icon);

  const contextMenu = Menu.buildFromTemplate([
    { label: 'Show/Hide', click: () => {
        if (mainWindow.isVisible()){
            mainWindow.hide();
        } else {
            mainWindow.show();
        }
    }},
    { label: 'Quit', click: () => {
      app.quit();
    } },
  ]);

  tray.setContextMenu(contextMenu);
  tray.setToolTip('Teamspeak Overlay');
};

app.whenReady().then(() => {
  createWindow();
});


app.on('before-quit', () => {
try {
  if (mainWindow) {
    mainWindow.destroy();
}
tray.destroy(); 
} catch (e) {
  console.error('Failed to destroy window', e);
}
});