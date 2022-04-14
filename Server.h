#pragma once

#include <winsock2.h>

class Server
{
private:
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	// std::map<unsigned int, SOCKET> sessions; 

	void printActiveAdapterAddresses(void);

public:
	Server(void);
	~Server(void);
	void mainLoop(void);
};

