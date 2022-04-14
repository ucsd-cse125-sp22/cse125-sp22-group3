#pragma once
using namespace std;
#include <string.h>
// the Packet struct that sent by client to server
struct ClientPacket {
	int move; 
    void serialize(char* data) {
        memcpy(data, this, sizeof(ClientPacket));
    }

    void deserialize(char* data) {
        memcpy(this, data, sizeof(ClientPacket));
    }
};

// the Packet struct that sent by server to client

struct ServerPacket {
	int move;
	bool valid; 
    void serialize(char* data) {
        memcpy(data, this, sizeof(ServerPacket));
    }

    void deserialize(char* data) {
        memcpy(this, data, sizeof(ServerPacket));
    }
};