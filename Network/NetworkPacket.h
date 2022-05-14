#pragma once
#include <cstring>
#include "..\glm.h"
#include "..\util.h"
#include "..\SoundEngine.h"

#define DEFAULT_PORT "2340"
#define DEFAULT_BUFLEN 1024

// the initial message that will be sent to client when waiting for other client to join
struct ClientWaitPacket {
	int client_joined;
	int max_client;

	void serializeTo(void* data) {
		memcpy(data, this, sizeof(ClientWaitPacket));
	}

	void deserializeFrom(const void* data) {
		memcpy(this, data, sizeof(ClientWaitPacket));
	}
};

// the Packet struct that sent by client to server
struct ClientPacket {
	bool justMoved = false;
	glm::vec2 movement{ 0,0 };
	InputCommands lastCommand;
	int player_idx; // only for server convenience, not to be filled by the client

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
	bool is_player;
};

struct SoundInfo
{
	SoundEnum sound;
	glm::vec3 sound_location; //TODO idk if we're actually using this but it might be cool
};

// the Packet struct that sent by server to client
struct ServerHeader {
	glm::mat4 player_transform;
	bool player_sprinting;
	int num_models;
	int num_sounds;
	bool ui_open;
	float balance[4];
};

inline void serverSerialize(char* out_buf, struct ServerHeader* head, struct ModelInfo* model_arr, struct SoundInfo* sound_arr) {
	memcpy(out_buf, head, sizeof(struct ServerHeader));
	memcpy(out_buf + sizeof(struct ServerHeader), model_arr, head->num_models * sizeof(struct ModelInfo));
	
	memcpy(out_buf + sizeof(struct ServerHeader) + (head->num_models * sizeof(struct ModelInfo)), sound_arr, head->num_sounds * sizeof(struct SoundInfo));
}

// client should free memory returned from head_buf_ptr and model_arr_buf_ptr
inline void serverDeserialize(char* in_buf, struct ServerHeader** head_buf_ptr, struct ModelInfo** model_arr_buf_ptr,
												struct SoundInfo** sound_arr_buf_ptr) {
	struct ServerHeader* head_buf = reinterpret_cast<struct ServerHeader*>(malloc(sizeof(struct ServerHeader)));
	memcpy(head_buf, in_buf, sizeof(struct ServerHeader));
	(*head_buf_ptr) = head_buf;

	int num_models = head_buf->num_models;

	struct ModelInfo* model_arr_buf = reinterpret_cast<struct ModelInfo*>(malloc(num_models * sizeof(struct ModelInfo)));
	memcpy(model_arr_buf, in_buf + sizeof(struct ServerHeader), num_models * sizeof(struct ModelInfo));
	(*model_arr_buf_ptr) = model_arr_buf;

	int num_sounds = head_buf->num_sounds;

	struct SoundInfo* sound_arr_buf = reinterpret_cast<struct SoundInfo*>(malloc(num_sounds * sizeof(struct SoundInfo)));
	memcpy(sound_arr_buf, in_buf + sizeof(struct ServerHeader) + (num_models * sizeof(struct ModelInfo)), num_sounds * sizeof(struct SoundInfo));
	(*sound_arr_buf_ptr) = sound_arr_buf;
}

inline size_t GetBufSize(struct ServerHeader* head) {
	return sizeof(struct ServerHeader) + (head->num_models * sizeof(ModelInfo)) + (head->num_sounds * sizeof(SoundInfo));
}
