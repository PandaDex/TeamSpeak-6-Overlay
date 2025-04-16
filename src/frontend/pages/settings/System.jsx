import React from "react";
import { Button } from "@/components/ui/button";

export default function SystemTab({ currentConfig, setCurrentConfig }) {
	return (
		<div className="w-full h-full flex flex-col gap-4">
			<div>
				<h2 className="font-semibold text-lg">Startup</h2>
				<p className="text-sm text-zinc-400 mb-2">Start overlay with windows</p>
				<Button
					variant={currentConfig.startWithWindows ? "destructive" : "default"}
					onClick={() =>
						setCurrentConfig((c) => ({
							...c,
							startWithWindows: !c.startWithWindows,
						}))
					}
				>
					{currentConfig.startWithWindows
						? "Remove from startup"
						: "Add to startup"}
				</Button>
			</div>
		</div>
	);
}
