#include "Client.h"

#include <stdio.h>
#include <stdlib.h>
#include <functional>

// Networking libraries
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// potentially needed
//#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "8686"
#define DEFAULT_BUFLEN 1024

// TODO: should fail be exit(1), or should it be handled silently?
Client::Client(const char* server_addr, const char* server_port)
{
	// initialize WinSock
	WSADATA wsaData;
	int startupStatus = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupStatus != 0)
	{
		printf("WSAStartup failed: %d\n", startupStatus);
		exit(1);
	}

	// prepare addrinfo request for server
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP; /* Establish TCP connection */

	/* get addrinfo for server */
	struct addrinfo* lookup_result = NULL; // result is the address info of the server
	int getaddrStatus = getaddrinfo(server_addr, server_port, &hints, &lookup_result);
	if (getaddrStatus != 0) {
		printf("getaddrinfo failed: %d\n", getaddrStatus);
		WSACleanup();
		exit(1);
	}

	// initialize socket for client
	for (struct addrinfo* s_addr_ptr = lookup_result; s_addr_ptr != NULL && ConnectSocket != INVALID_SOCKET; s_addr_ptr = s_addr_ptr->ai_next) {
		// creating a socket for the client to connect to the server
		ConnectSocket = socket(s_addr_ptr->ai_family, s_addr_ptr->ai_socktype, s_addr_ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			exit(1);
		}

		// connect to the server
		int connectStatus = connect(ConnectSocket, s_addr_ptr->ai_addr, (int)s_addr_ptr->ai_addrlen);
		if (connectStatus == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
		}
	}

	freeaddrinfo(lookup_result);

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
		exit(1);
	}

	// TODO deal with remaining data, before server also closes connection

	closesocket(ConnectSocket);
	WSACleanup();
}

// TODO more graceful error handling
void Client::syncWithServer(std::function<void()> callback) {
	int recvStatus;
	do {
		// sending data (inputs)
		const char* dummyData = "CSE 125 Group 3 Network Team";
		int sendStatus = send(ConnectSocket, dummyData, strlen(dummyData), 0);
		if (sendStatus == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			exit(1);
		}
		else {
			printf("Bytes Sent: %ld\n", sendStatus);
		}

		//receiving data (updated state)
		char recvbuf[DEFAULT_BUFLEN];
		recvStatus = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (recvStatus < 0)
		{
			printf("recv failed: %d\n", WSAGetLastError());
		}
		else if (recvStatus == 0)
		{
			printf("Connection closed\n");
			closesocket(ConnectSocket);
			WSACleanup();
			exit(1);
		}
		else
		{
			printf("Bytes Received: %ld\n", recvStatus);
		}

		callback();
	} while (recvStatus > 0);
}