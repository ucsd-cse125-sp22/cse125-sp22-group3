#pragma once
#include <functional>
#include <winsock2.h>
#include "NetworkPacket.h"

class Client
{
private:
	SOCKET ConnectSocket = INVALID_SOCKET;

public:
	static bool waitForOtherToJoin; 
	Client(const char* server_addr, const char* server_port);
	~Client(void);
	int syncWithServer(const void* send_buf, size_t send_len,
		std::function<void(char* recv_buf, size_t recv_len)> callback);
	void syncGameReadyToStart(std::function<void(ClientWaitPacket in_wait_packet)> callback);
	std::vector<ModelEnum> syncCharacterSelection(int num_clients,
		std::function<ClientCharacterPacket(ServerCharacterPacket recv_packet)> callback);
};

