#pragma once
#include <functional>
#include <winsock2.h>

#include "NetworkPacket.h"
#include "../GameManager.h"

class Server
{
private:
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	void printActiveAdapterAddresses(void);

public:
	Server(void);
	~Server(void);
	void mainLoop(std::function<std::pair<char*, int>(ClientPacket cpacket)> main_code);
};

