import React, { useState, useEffect } from "react";
import SpeakingClient from "../components/SpeakingClient";
import { Toaster, toast } from "sonner";
import JoiningClient from "../components/JoiningClient";

function View() {
  const [speakingClients, setSpeakingClients] = useState([]);
  const [connectingClients, setConnectingClients] = useState([]);

  useEffect(() => {
    const ws = new WebSocket("ws://localhost:5899/");
    let clients = [];
    var wasAuth = false;
    var currentChannel = "";
    var currentId = "";

    ws.onopen = () => {
      console.log("Connected to WebSocket server");
      const apiKey = localStorage.getItem("api_key") || "";
      const payload = {
        type: "auth",
        payload: {
          identifier: "dex.teamspeak6.overlay",
          version: "1.0.0",
          name: "Teamspeak Overlay",
          description: "Shows speaking clients on screen",
          content: {
            apiKey: apiKey,
          },
        },
      };
      ws.send(JSON.stringify(payload));

      toast.success("Connected", {
        richColors: true,
        duration: 2000,
        position: "top-center",
        style: {
          backgroundColor: "#2f2f2f",
          borderColor: "#4f4f4f",
          width: "154.4px",
        },
      });
    };

    ws.onmessage = (message) => {
      if (message.data) {
        const data = JSON.parse(message.data);

        if (data.type === "auth") {
          clients = data.payload.connections[0].clientInfos.map((client) => ({
            id: client.id,
            nickname: client.properties.nickname,
            avatar: client.properties.myteamspeakAvatar.replace("2,", ""),
          }));

          localStorage.setItem("api_key", data.payload.apiKey);
          currentId = data.payload.connections[0].clientId;
          var d = data.payload.connections[0].clientInfos.filter(
            (client) => client.id === data.payload.connections[0].clientId
          )[0].channelId;

          currentChannel = parseInt(d);
          wasAuth = true;
        }

        if (data.type === "clientMoved") {
          if (currentId !== data.payload.clientId) return;
          currentChannel = parseInt(data.payload.newChannelId);
        }

        if (data.type === "clientChannelGroupChanged") {
          if (parseInt(data.payload.channelId) !== currentChannel) return;
          var client = clients.filter(
            (client) => client.id === data.payload.clientId
          )[0];

          setConnectingClients((prev) => [...prev, client]);
          setInterval(() => {
            setConnectingClients((prev) =>
              prev.filter((client2) => client2.id !== client.id)
            );
          }, 1500);
        }

        if (data.type === "clientPropertiesUpdated") {
          clients = clients.map((client) =>
            client.id === data.payload.clientId
              ? {
                  id: data.payload.clientId,
                  nickname: data.payload.properties.nickname,
                  avatar: data.payload.properties.myteamspeakAvatar.replace(
                    "2,",
                    ""
                  ),
                }
              : client
          );
        }

        if (data.type === "talkStatusChanged") {
          if (data.payload.status === 1) {
            setSpeakingClients((prev) => [
              ...prev,
              clients.find((client) => client.id === data.payload.clientId),
            ]);
          } else {
            setSpeakingClients((prev) =>
              prev.filter((client) => client.id !== data.payload.clientId)
            );
          }
        }
      }
    };

    ws.onerror = (error) => {
      console.error("WebSocket Error:", error);
      toast.error("WebSocket Error", {
        richColors: true,
        duration: 2000,
        position: "top-center",
        style: {
          backgroundColor: "#2f2f2f",
          borderColor: "#4f4f4f",
          width: "154.4px",
        },
      });
    };

    ws.onclose = () => {
      console.log("WebSocket connection closed");
      if (!wasAuth) {
        localStorage.removeItem("api_key");
      }

      window.location.reload();

      toast.info("Disconnected", {
        richColors: true,
        duration: 2000,
        position: "top-center",
        style: {
          backgroundColor: "#2f2f2f",
          borderColor: "#4f4f4f",
          width: "154.4px",
        },
      });
    };

    return () => {
      ws.close();
    };
  }, []);

  return (
    <div className="w-full h-full flex flex-col gap-1 py-1 pl-1">
      <Toaster />
      {connectingClients.map(
        (client) => (
          console.log(client),
          (<JoiningClient key={client?.id} nickname={client?.nickname} />)
        )
      )}
      {speakingClients.map((client) => (
        <SpeakingClient
          key={client?.id}
          avatar={client?.avatar}
          username={client?.nickname}
        />
      ))}
    </div>
  );
}

export default View;
