#pragma once
#include <functional>
#include <winsock2.h>

#include "NetworkPacket.h"
#include "../GameManager.h"

#define DEFAULT_NUM_CLIENTS 4

class Server
{
private:
	SOCKET ListenSocket = INVALID_SOCKET;
	std::vector<SOCKET> ClientSocketVec;
	
	void printActiveAdapterAddresses(void);
	std::string getRemoteAddressString(SOCKET remote_socket);
	std::string getCharacterFriendlyName(ModelEnum model);

public:
	Server(void);
	~Server(void);

	void WaitForClients();
	std::vector<ModelEnum> CharacterSelection();
	void mainLoop(std::function<std::vector<std::pair<char*, int>>(std::vector<ClientPacket> client_packet_vec)> main_code);

	int num_clients = DEFAULT_NUM_CLIENTS;
};

