const { ipcRenderer } = window.require("electron");

const log = (message, type = 'INFO') => {
    console.log(message);
    ipcRenderer.send('log', message, type);
}

export { log };