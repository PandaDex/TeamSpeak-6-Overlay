import React from "react";

export default function AboutTab({ version }) {
	return (
		<div className="w-full h-full flex flex-col items-center justify-center">
			<img
				src="./icons/icon@1x.png"
				alt="Product Icon"
				className="w-16 h-16 mb-1"
			/>
			<h1 className="font-semibold text-xl">TeamSpeak Overlay</h1>
			<p className="text-sm text-red-400">
				Not affiliated with TeamSpeak Systems, Inc.
			</p>
			<p className="text-sm text-zinc-400">Developer: PandaDex</p>
			<p className="text-sm text-zinc-400">Version: {version}</p>
		</div>
	);
}
