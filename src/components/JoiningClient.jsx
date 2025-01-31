import React from "react";

function JoiningClient({ nickname }) {
  return (
    <div className="flex gap-1 items-center bg-black/45 w-fit px-1.5 py-0.5 rounded-md">
      <p className="aspect-square text-xl h-5 flex justify-center items-center font-bold text-green-500">
        +
      </p>
      <p className="text-base text-white">{nickname}</p>
    </div>
  );
}

export default JoiningClient;
