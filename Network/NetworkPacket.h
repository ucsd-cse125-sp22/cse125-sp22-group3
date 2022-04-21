#pragma once
#include <cstring>
#include "..\packages\glm.0.9.9.800\build\native\include\glm\vec2.hpp"
#include "..\util.h"

// the Packet struct that sent by client to server
struct ClientPacket {
	bool justMoved = false;
	glm::vec2 movement{ 0,0 };
	InputCommands lastCommand;

	void serializeTo(void* data) {
		memcpy(data, this, sizeof(ClientPacket));
	}

	void deserializeFrom(const void* data) {
		memcpy(this, data, sizeof(ClientPacket));
	}
};

// the Packet struct that sent by server to client
struct ServerPacket {
	bool justMoved = false;
	glm::vec2 movement{ 0,0 };
	InputCommands lastCommand;

	void serializeTo(void* data) {
		memcpy(data, this, sizeof(ServerPacket));
	}

	void deserializeFrom(const void* data) {
		memcpy(this, data, sizeof(ServerPacket));
	}
};