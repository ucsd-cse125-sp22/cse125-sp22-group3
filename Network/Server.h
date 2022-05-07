#pragma once
#include <functional>
#include <winsock2.h>

#include "NetworkPacket.h"
#include "../GameManager.h"

class Server
{
private:
	SOCKET ListenSocket = INVALID_SOCKET;
	std::vector<SOCKET> ClientSocketVec;

	void printActiveAdapterAddresses(void);
	std::string getRemoteAddressString(SOCKET remote_socket);

public:
	Server(void);
	~Server(void);
	void mainLoop(std::function<std::vector<std::pair<char*, int>>(std::vector<ClientPacket> client_packet_vec)> main_code);
};

