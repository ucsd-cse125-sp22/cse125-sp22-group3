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
	int syncWithServer(const char* send_buf, size_t send_len,
		std::function<void(const char* recv_buf, size_t recv_len)> callback);
};

