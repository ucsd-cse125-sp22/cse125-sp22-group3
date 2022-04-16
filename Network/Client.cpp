#include "Client.h"
#include "NetworkPacket.h"
#include <stdio.h>
#include <stdlib.h>
#include <functional>

// Networking libraries
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 1024

Client::Client(const char* server_addr, const char* server_port)
{
	// initialize Winsock 2.2
	WSADATA wsaData;
	int startupStatus = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupStatus != 0) {
		printf("WSAStartup failed: %d\n", startupStatus);
		exit(1);
	}

	// hints for request to get server address
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// resolve server address and port info
	struct addrinfo* result = NULL;
	int getaddrStatus = getaddrinfo(server_addr, server_port, &hints, &result);
	if (getaddrStatus != 0) { // can't resolve server hostname / IP address
		printf("getaddrinfo failed: %d\n", getaddrStatus);
		WSACleanup();
		exit(1);
	}

	// attempt all addresses from getaddrinfo's result
	for (struct addrinfo* addr = result; addr != NULL; addr = addr->ai_next) {
		// creating a socket for the client conforming to current address's protocols
		ConnectSocket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			continue;
		}

		// connect to the current address
		int connectStatus = connect(ConnectSocket, addr->ai_addr, (int)addr->ai_addrlen);
		if (connectStatus == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
		}
		else {
			break;
		}
	}

	freeaddrinfo(result); // no longer needed

	// if connection failed
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		exit(1);
	}

	bool isNonblocking = false; // TODO revisit later, depending on game architecture
	if (isNonblocking) {
		// set the mode of the socket to be nonblocking
		u_long iMode = 1;
		int ioctlResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
		if (ioctlResult == SOCKET_ERROR) {
			printf("ioctlsocket failed with error :%d/n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			exit(1);
		}
	}

	// disable Nagle's algorithm
	char value = 1;
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
}

Client::~Client(void)
{
	int shutdownStatus = shutdown(ConnectSocket, SD_SEND);
	if (shutdownStatus == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	// TODO deal with remaining data, before server also closes connection

	closesocket(ConnectSocket);
	WSACleanup();
}

/*
 * Returns bytes received from server. If return value is 0, server has closed connection.
 * Delete the client object immediately.
 */
int Client::syncWithServer(const char* send_buf, size_t send_len,
	std::function<void(const char* recv_buf, size_t recv_len)> callback)
{
	// sending data (inputs)
	int sendStatus = send(ConnectSocket, send_buf, send_len, 0);
	if (sendStatus == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		exit(1); // TODO more graceful error handling
	}
	else {
		printf("Client bytes sent: %ld\n", sendStatus);
	}

	//receiving data (updated state)
	char recvbuf[DEFAULT_BUFLEN];
	int recvStatus = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	if (recvStatus == SOCKET_ERROR) {
		printf("recv failed: %d\n", WSAGetLastError());
	}
	else if (recvStatus == 0) {
		printf("Connection closed by server\n");
	}
	else {
		printf("Client bytes received: %ld\n", recvStatus);
		callback(recvbuf, recvStatus);
	}

	return recvStatus;
}

