import { defineConfig } from 'vite';
import tailwindcss from '@tailwindcss/vite'
import path from 'node:path';

// https://vitejs.dev/config
export default defineConfig({
  root: path.resolve(__dirname, "src", "frontend"),
  plugins: [
    tailwindcss(),
  ],
  resolve: {
    alias: {
      "@": path.resolve(__dirname, "./src/frontend"),
    },
  },
});
