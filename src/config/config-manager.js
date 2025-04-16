import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

export const DEFAULT_CONFIG = {
    position: 'tl',
    padding: 10,
    startWithWindows: false,
    opacity: 100,
};

export function getConfigPath() {
    return os.platform() === "win32"
        ? path.join(os.homedir(), 'AppData/Roaming/teamspeak-overlay/config.json')
        : path.join(os.homedir(), '.config/teamspeak-overlay/config.json');
}

export function initializeConfig(CONFIG_PATH) {
    console.log('[CONFIG_MANAGER] [INFO] > Initializing configuration');
    try {
        if (fs.existsSync(CONFIG_PATH)) {
            console.log('[CONFIG_MANAGER] [INFO] > Loading existing config file');
            const data = JSON.parse(fs.readFileSync(CONFIG_PATH, 'utf8'));
            return { ...DEFAULT_CONFIG, ...data };
        } else {
            console.log('[CONFIG_MANAGER] [INFO] > Creating new config file with defaults');
            fs.writeFileSync(CONFIG_PATH, JSON.stringify(DEFAULT_CONFIG), { encoding: 'utf-8' });
            return DEFAULT_CONFIG;
        }
    } catch (error) {
        console.error('[CONFIG_MANAGER] [ERROR] > Failed to initialize config:', error);
        throw error;
    }
}