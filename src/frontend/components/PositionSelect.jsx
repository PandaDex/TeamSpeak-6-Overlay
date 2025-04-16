import React from "react";

export default function PositionSelect({ valve, onChange }) {
	return (
		<div className="aspect-video h-[200px] border border-zinc-800 mt-4 rounded-xl grid grid-cols-2 grid-rows-2 gap-2 p-2">
			<button
				type="button"
				onClick={() => onChange("tl")}
				className={`rounded-tl-lg border-l-6 border-t-6 ${
					valve === "tl"
						? "border-white text-white"
						: "border-zinc-800 text-zinc-600"
				} hover:border-zinc-300 hover:text-zinc-300 ease-in-out duration-100 cursor-pointer  flex items-center justify-center`}
			>
				top left
			</button>
			<button
				type="button"
				onClick={() => onChange("tr")}
				className={`rounded-tr-lg border-r-6 border-t-6 ${
					valve === "tr"
						? "border-white text-white"
						: "border-zinc-800 text-zinc-600"
				} hover:border-zinc-300 hover:text-zinc-300 ease-in-out duration-100 cursor-pointer  flex items-center justify-center`}
			>
				top right
			</button>
			<button
				type="button"
				onClick={() => onChange("bl")}
				className={`rounded-bl-lg border-l-6 border-b-6 ${
					valve === "bl"
						? "border-white text-white"
						: "border-zinc-800 text-zinc-600"
				} hover:border-zinc-300 hover:text-zinc-300 ease-in-out duration-100 cursor-pointer  flex items-center justify-center`}
			>
				bottom left
			</button>
			<button
				type="button"
				onClick={() => onChange("br")}
				className={`rounded-br-lg border-r-6 border-b-6 ${
					valve === "br"
						? "border-white text-white"
						: "border-zinc-800 text-zinc-600"
				} hover:border-zinc-300 hover:text-zinc-300 ease-in-out duration-100 cursor-pointer  flex items-center justify-center`}
			>
				bottom right
			</button>
		</div>
	);
}
