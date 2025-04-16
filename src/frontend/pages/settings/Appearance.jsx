import React from "react";
import PositionSelect from "@/components/PositionSelect";
import { Input } from "@/components/ui/input";

export default function AppearanceTab({ currentConfig, setCurrentConfig }) {
	return (
		<div className="w-full h-full flex flex-col  gap-4">
			<div>
				<h2 className="font-semibold text-lg">Overlay Position</h2>
				<p className="text-sm text-zinc-400">
					Change the position of the overlay on your screen.
				</p>
				<PositionSelect
					valve={currentConfig.position}
					onChange={(t) => {
						setCurrentConfig((c) => ({ ...c, position: t }));
					}}
				/>
			</div>
			<div>
				<h2 className="font-semibold text-lg">Overlay Padding</h2>
				<p className="text-sm text-zinc-400 mb-4">
					Distance between the edge of the screen and the overlay.
				</p>
				<Input
					type="number"
					className="w-fit"
					value={
						!Number.isNaN(currentConfig.padding) ? currentConfig.padding : 0
					}
					onChange={(e) =>
						setCurrentConfig((c) => ({
							...c,
							padding: Number.parseInt(e.target.value),
						}))
					}
				/>
			</div>
			<div>
				<h2 className="font-semibold text-lg">Overlay Opacity</h2>
				<p className="text-sm text-zinc-400 mb-4">Opacity of the overlay.</p>
				<Input
					type="number"
					className="w-fit"
					value={
						!Number.isNaN(currentConfig.opacity) ? currentConfig.opacity : 0
					}
					onChange={(e) =>
						setCurrentConfig((c) => ({
							...c,
							opacity: Number.parseInt(e.target.value),
						}))
					}
				/>
			</div>
		</div>
	);
}
