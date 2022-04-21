#pragma once
#include <cstring>
#include "..\glm.h"
#include "..\util.h"

// the Packet struct that sent by client to server
struct ClientPacket {
	bool justMoved = false;
	glm::vec2 movement{ 0,0 };
	InputCommands lastCommand;
	int player_index;

	void serializeTo(void* data) {
		memcpy(data, this, sizeof(ClientPacket));
	}

	void deserializeFrom(const void* data) {
		memcpy(this, data, sizeof(ClientPacket));
	}
};

struct ModelInfo {
	int model_id;
	ModelEnum model;
	glm::mat4 parent_transform;
};

// the Packet struct that sent by server to client
struct ServerHeader {
	int player_model_id;
	int num_models;
};

inline void serverSerialize(char* out_buf, struct ServerHeader *head, struct ModelInfo *model_arr) {
	memcpy(out_buf, head, sizeof(struct ServerHeader));
	memcpy(out_buf + sizeof(struct ServerHeader), model_arr, head->num_models * sizeof(struct ModelInfo));
}

inline void serverDeserialize(char* in_buf, struct ServerHeader *head_buf, struct ModelInfo *model_arr_buf) {
	memcpy(head_buf, in_buf, sizeof(struct ServerHeader));
	memcpy(model_arr_buf, in_buf + sizeof(struct ServerHeader), head_buf->num_models * sizeof(struct ModelInfo));
}

inline size_t GetBufSize(struct ServerHeader *head) {
	return sizeof(struct ServerHeader) + head->num_models * sizeof(ModelInfo);
}


/*
struct ModelInfo {
  int model_id;
  ModelEnum model;
  glm::mat4 parent_transform;
}

struct ServerPacket {
  int player_model_id;
  int num_models;
//   ModelInfo[num_models] models; since not fixed size, goes outside the struct
}*/
