#include "Server.h"

#include <stdio.h>
#include <stdlib.h>

// Networking libraries
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "8686"
#define DEFAULT_BUFLEN 1024

Server::Server(void) {
	// initialize Winsock 2.2
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
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP; /* Establish TCP connection */
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	struct addrinfo* result = NULL;
	int getaddrStatus = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result); // TODO change default port
	if (getaddrStatus != 0) {
		printf("getaddrinfo failed with error: %d\n", getaddrStatus);
		WSACleanup();
		exit(1);
	}

	// Create a passive SOCKET for the server
	ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}

	bool isNonblocking = false; // TODO revisit later, depending on game architecture
	if (isNonblocking) {
		// set the mode of the socket to be nonblocking

		u_long iMode = 1;
		int ioctlStatus = ioctlsocket(ListenSocket, FIONBIO, &iMode);
		if (ioctlStatus == SOCKET_ERROR) {
			printf("ioctlsocket failed with error :%d/n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			exit(1);
		}
	}

	// Setup the TCP listening socket
	int bindStatus = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (bindStatus == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	// no longer need address information
	freeaddrinfo(result);

	// start listening for new clients attempting to connect
	int listenStatus = listen(ListenSocket, SOMAXCONN);
	if (listenStatus == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
}

Server::~Server(void) {
	// shutdown the send half of the connection since no more data will be sent
		int shutdownCode = shutdown(ClientSocket, SD_SEND);
	if (shutdownCode == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		exit(1);
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
}

void Server::mainLoop(void)
{
	ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket != INVALID_SOCKET)
	{
		// disable Nagle on the client's socket
		char value = 1;
		setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		// LATER: insert new client into session id table
		// sessions.insert( pair<unsigned int, SOCKET>(id, ClientSocket) );
	}

	char buffer[DEFAULT_BUFLEN];
	while (true) {
		/* Receive data */
		int data_length;
		data_length = recv(ClientSocket, buffer, DEFAULT_BUFLEN, 0);
		if (data_length == 0) {
			printf("Connection closed\n");
			closesocket(ClientSocket);
		}
		else if (data_length < 0) {
			// no data received
			continue;
			// TODO nonblocking stuff, deal with later
		}

		if (data_length > 0) { // TODO remove clause when using non-blocking
			//echo back the data recieve to the client 
			int sendStatus = send(ClientSocket, buffer, DEFAULT_BUFLEN, 0);
			if (sendStatus == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				exit(1);
			}
			else {
				printf("Bytes Sent: %ld\n", sendStatus);
			}
		}
	}
}