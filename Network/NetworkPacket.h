#pragma once
#include <cstring>
#include "..\glm.h"
#include "..\util.h"

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

struct ClientCharacterPacket {
	ModelEnum character;
	
	void serializeTo(void* data) {
		memcpy(data, this, sizeof(ClientCharacterPacket));
	}

	void deserializeFrom(const void* data) {
		memcpy(this, data, sizeof(ClientCharacterPacket));
	}
};

struct ServerCharacterPacket {
	int client_idx;
	ModelEnum current_char_selections[4];

	void serializeTo(void* data) {
		memcpy(data, this, sizeof(ServerCharacterPacket));
	}

	void deserializeFrom(const void* data) {
		memcpy(this, data, sizeof(ServerCharacterPacket));
	}
};

// the Packet struct that sent by client to server
struct ClientPacket {
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

struct PendingDeleteInfo
{
	uintptr_t model_id;
};

// the Packet struct that sent by server to client
struct ServerHeader {
	int num_models;
	int num_sounds;
	int num_pending_delete;
	float balance[4];
	double time_remaining_seconds;
	double time_max_seconds;
	glm::mat4 player_transform;
	double stamina_bar;
	bool player_sprinting;
	bool ui_open;
};

inline void serverSerialize(char* out_buf, struct ServerHeader* head, struct ModelInfo* model_arr, struct SoundInfo* sound_arr, struct PendingDeleteInfo* pending_arr) {
	int starting_pos = 0;

	memcpy(out_buf + starting_pos, head, sizeof(struct ServerHeader));
	starting_pos += sizeof(struct ServerHeader);

	memcpy(out_buf + starting_pos, model_arr, head->num_models * sizeof(struct ModelInfo));
	starting_pos += head->num_models * sizeof(struct ModelInfo);

	memcpy(out_buf + starting_pos, sound_arr, head->num_sounds * sizeof(struct SoundInfo));
	starting_pos += head->num_sounds * sizeof(struct SoundInfo);

	memcpy(out_buf + starting_pos, pending_arr, head->num_pending_delete * sizeof(struct PendingDeleteInfo));
	starting_pos += head->num_pending_delete * sizeof(struct PendingDeleteInfo);
}

// client should free memory returned from head_buf_ptr and model_arr_buf_ptr
inline void serverDeserialize(char* in_buf, struct ServerHeader** head_buf_ptr, struct ModelInfo** model_arr_buf_ptr,
											struct SoundInfo** sound_arr_buf_ptr, struct PendingDeleteInfo** pending_arr_buf_ptr) {
	int starting_pos = 0;
	struct ServerHeader* head_buf = reinterpret_cast<struct ServerHeader*>(malloc(sizeof(struct ServerHeader)));
	memcpy(head_buf, in_buf + starting_pos, sizeof(struct ServerHeader));
	(*head_buf_ptr) = head_buf;
	starting_pos += sizeof(struct ServerHeader);

	int num_models = head_buf->num_models;
	struct ModelInfo* model_arr_buf = reinterpret_cast<struct ModelInfo*>(malloc(num_models * sizeof(struct ModelInfo)));
	memcpy(model_arr_buf, in_buf + starting_pos, num_models * sizeof(struct ModelInfo));
	(*model_arr_buf_ptr) = model_arr_buf;
	starting_pos += num_models * sizeof(struct ModelInfo);

	int num_sounds = head_buf->num_sounds;
	struct SoundInfo* sound_arr_buf = reinterpret_cast<struct SoundInfo*>(malloc(num_sounds * sizeof(struct SoundInfo)));
	memcpy(sound_arr_buf, in_buf + starting_pos, num_sounds * sizeof(struct SoundInfo));
	(*sound_arr_buf_ptr) = sound_arr_buf;
	starting_pos += num_sounds * sizeof(struct SoundInfo);

	int num_pending_delete = head_buf->num_pending_delete;
	struct PendingDeleteInfo* pending_arr_buf = reinterpret_cast<struct PendingDeleteInfo*>(malloc(num_pending_delete * sizeof(struct PendingDeleteInfo)));
	memcpy(pending_arr_buf, in_buf + starting_pos, num_pending_delete * sizeof(struct PendingDeleteInfo));
	(*pending_arr_buf_ptr) = pending_arr_buf;
	starting_pos += num_pending_delete * sizeof(struct PendingDeleteInfo);
}

inline size_t GetBufSize(struct ServerHeader* head) {
	return sizeof(struct ServerHeader) + (head->num_models * sizeof(ModelInfo)) + (head->num_sounds * sizeof(SoundInfo)) + (head->num_pending_delete * sizeof(struct PendingDeleteInfo));
}
