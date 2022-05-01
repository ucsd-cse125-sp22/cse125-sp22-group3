#pragma once
#include <cstring>
#include "..\glm.h"
#include "..\util.h"

#define DEFAULT_PORT "2340"
#define DEFAULT_BUFLEN 1024
#define NUM_CLIENTS 1

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

struct ModelInfo {
	uintptr_t model_id;
	ModelEnum model;
	AniMode modelAnim;
	glm::mat4 parent_transform;
};

// the Packet struct that sent by server to client
struct ServerHeader {
	glm::mat4 player_transform;
	int num_models;
};

inline void serverSerialize(char* out_buf, struct ServerHeader* head, struct ModelInfo* model_arr) {
	memcpy(out_buf, head, sizeof(struct ServerHeader));
	memcpy(out_buf + sizeof(struct ServerHeader), model_arr, head->num_models * sizeof(struct ModelInfo));
}

// client should free memory returned from head_buf_ptr and model_arr_buf_ptr
inline void serverDeserialize(char* in_buf, struct ServerHeader** head_buf_ptr, struct ModelInfo** model_arr_buf_ptr) {
	struct ServerHeader* head_buf = reinterpret_cast<struct ServerHeader*>(malloc(sizeof(struct ServerHeader)));
	memcpy(head_buf, in_buf, sizeof(struct ServerHeader));
	(*head_buf_ptr) = head_buf;

	int num_models = head_buf->num_models;

	struct ModelInfo* model_arr_buf = reinterpret_cast<struct ModelInfo*>(malloc(num_models * sizeof(struct ModelInfo)));
	memcpy(model_arr_buf, in_buf + sizeof(struct ServerHeader), num_models * sizeof(struct ModelInfo));
	(*model_arr_buf_ptr) = model_arr_buf;
}

inline size_t GetBufSize(struct ServerHeader* head) {
	return sizeof(struct ServerHeader) + head->num_models * sizeof(ModelInfo);
}
