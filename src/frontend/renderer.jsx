/**
 * This file will automatically be loaded by vite and run in the "renderer" context.
 * To learn more about the differences between the "main" and the "renderer" context in
 * Electron, visit:
 *
 * https://electronjs.org/docs/tutorial/application-architecture#main-and-renderer-processes
 *
 * By default, Node.js integration in this file is disabled. When enabling Node.js integration
 * in a renderer process, please be aware of potential security implications. You can read
 * more about security risks here:
 *
 * https://electronjs.org/docs/tutorial/security
 *
 * To enable Node.js integration in this file, open up `main.js` and enable the `nodeIntegration`
 * flag:
 *
 * ```
 *  // Create the browser window.
 *  mainWindow = new BrowserWindow({
 *    width: 800,
 *    height: 600,
 *    webPreferences: {
 *      nodeIntegration: true
 *    }
 *  });
 * ```
 */

import "./index.css";

import { createRoot } from "react-dom/client";
import React from "react";
import { HashRouter as Router, Routes, Route } from "react-router-dom";
import Settings from "@/pages/Settings";
import View from "@/pages/View";

createRoot(document.getElementById("root")).render(
	<Router>
		<Routes>
			<Route path="/" element={<View />} />
			<Route path="/settings" element={<Settings />} />
		</Routes>
	</Router>,
);
