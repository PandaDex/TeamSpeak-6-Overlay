import React, { useEffect, useState } from "react";
import { Button } from "@/components/ui/button";
import AppearanceTab from "@/pages/settings/Appearance";
import SystemTab from "@/pages/settings/System";
import AboutTab from "@/pages/settings/About";
import { Loader2 } from "lucide-react";

const { ipcRenderer } = window.require("electron");

function Settings() {
	const [activeTab, setActiveTab] = useState("appearance");
	const [startConfig, setStartConfig] = useState(null);
	const [currentConfig, setCurrentConfig] = useState(null);
	const [version, setVersion] = useState(null);

	useEffect(() => {
		ipcRenderer.send("get-config");
		ipcRenderer.send("get-version");
		ipcRenderer.on("config", (_, data) => {
			setStartConfig(data);
			setCurrentConfig(data);
		});

		ipcRenderer.on("version", (_, data) => {
			setVersion(data);
		});

		return () => {
			ipcRenderer.removeAllListeners("config");
		};
	}, []);

	return startConfig !== null ? (
		<div className="w-full dark text-white h-full bg-zinc-950 p-6 flex flex-col">
			<title>TeamSpeak Overlay Settings</title>
			<h1 className="font-semibold text-xl">Settings</h1>
			<p className="text-sm text-zinc-400">
				Customize overlay any way you want!
			</p>
			<div className="grow h-[1px] my-5 rounded-full bg-zinc-800" />
			<div className="w-full h-full flex">
				<aside className="w-1/5 h-full flex flex-col gap-1">
					<Button
						variant="ghost"
						className={`justify-start ${
							activeTab === "appearance" ? "bg-zinc-900" : ""
						}`}
						onClick={() => setActiveTab("appearance")}
					>
						Appearance
					</Button>
					<Button
						variant="ghost"
						className={`justify-start ${
							activeTab === "system" ? "bg-zinc-900" : ""
						}`}
						onClick={() => setActiveTab("system")}
					>
						System
					</Button>
					<Button
						variant="ghost"
						className={`justify-start ${
							activeTab === "about" ? "bg-zinc-900" : ""
						}`}
						onClick={() => setActiveTab("about")}
					>
						About
					</Button>
				</aside>
				<main className="w-4/5 flex flex-col pl-6 min-h-0">
					<div className="flex-1 overflow-y-auto">
						<TabRouter
							tab={activeTab}
							{...{ currentConfig, setCurrentConfig, version }}
						/>
					</div>
					{JSON.stringify(startConfig) !== JSON.stringify(currentConfig) && (
						<div className="w-full bg-zinc-950 border rounded-lg flex items-center justify-between px-2 py-2 mt-2">
							<p className="text-base font-medium">
								Relaunch is required to apply changes!
							</p>
							<div className="grow" />
							<Button
								size="sm"
								className="mr-2"
								onClick={() => {
									setCurrentConfig(startConfig);
								}}
								variant="secondary"
							>
								Restore Previous
							</Button>
							<Button
								size="sm"
								onClick={() => {
									ipcRenderer.send("save-config", currentConfig);
								}}
							>
								Save And Restart
							</Button>
						</div>
					)}
				</main>
			</div>
		</div>
	) : (
		<div className="w-full h-full flex flex-col items-center justify-center">
			<Loader2 className="animate-spin" />
			<p>Loading...</p>
		</div>
	);
}

const TabRouter = ({ tab, currentConfig, setCurrentConfig, version = "" }) => {
	switch (tab) {
		case "appearance":
			return <AppearanceTab {...{ currentConfig, setCurrentConfig }} />;
		case "system":
			return <SystemTab {...{ currentConfig, setCurrentConfig }} />;
		case "about":
			return <AboutTab {...{ version }} />;
		default:
			return <AppearanceTab {...{ currentConfig, setCurrentConfig }} />;
	}
};

export default Settings;
