import React from "react";

function SpeakingClient({ avatar, username }) {
	return (
		<div className="flex gap-1 items-center bg-black/45 w-fit px-1.5 py-0.5 rounded-md">
			<img
				src={avatar || "./default.png"}
				className="rounded-full aspect-square h-5"
				alt="Avatar"
			/>
			<p className="text-base text-white">{username}</p>
		</div>
	);
}

export default SpeakingClient;
