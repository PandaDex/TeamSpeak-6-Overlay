import { ipcMain, app } from 'electron';
import fs from 'node:fs';
import os from 'node:os';

export function setupIpcHandlers(config, CONFIG_PATH) {
    ipcMain.on('get-config', (event) => {
        event.reply('config', config);
    });

    ipcMain.on('get-version', (event) => {
        event.reply('version', `${app.getVersion()}-${os.platform()}`);
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

    ipcMain.on('log', (_, message, type) => {
        console.log(`[FRONTEND] [${type}] > ${message}`);
    });
}