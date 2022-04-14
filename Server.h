#pragma once

#include <winsock2.h>
#define DEFAULT_PORT "8686" // TODO change default port
#define DEFAULT_BUFLEN 1024


class Server
{
private:
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	// std::map<unsigned int, SOCKET> sessions; 
	char network_data[DEFAULT_BUFLEN];


	void printActiveAdapterAddresses(void);

public:
	Server(void);
	~Server(void);
	void mainLoop(void);
};

