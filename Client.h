#pragma once
#include <functional>
#include <winsock2.h>

class Client
{
private:
	SOCKET ConnectSocket = INVALID_SOCKET;

public:
	Client(const char* server_addr, const char* server_port);
	~Client(void);
	void syncWithServer(std::function<void()> callback);
};

