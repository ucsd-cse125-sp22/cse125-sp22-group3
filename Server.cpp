#include "Server.h"
#include "NetworkPacket.h"

#include <stdio.h>
#include <stdlib.h>

// Networking libraries
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")


Server::Server(void)
{
	// initialize Winsock 2.2
	WSADATA wsaData;
	int startupStatus = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupStatus != 0) {
		printf("WSAStartup failed: %d\n", startupStatus);
		exit(1);
	}

	// prepare addrinfo request for server
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// resolve the local address and port for the server
	struct addrinfo* result = NULL;
	int getaddrStatus = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
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

	// bind passive socket to the local address
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

Server::~Server(void)
{
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
	ClientSocket = accept(ListenSocket, NULL, NULL); // TODO ClientSocket handling can be multithreaded
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		return; // TODO error condition for this
	}

	// disable Nagle on the client socket
	char value = 1;
	setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

	// TODO insert new client into session id table
	// sessions.insert(pair<unsigned int, SOCKET>(id, ClientSocket));

	/*char buffer[DEFAULT_BUFLEN];
	while (true) { // TODO 
		// receive data from client
		int recvStatus = recv(ClientSocket, buffer, DEFAULT_BUFLEN, 0);
		if (recvStatus == SOCKET_ERROR) {
			printf("recv failed: %d\n", WSAGetLastError());
			continue;
		}
		else if (recvStatus == 0) {
			printf("Connection closed\n");
			closesocket(ClientSocket);
			return; // TODO only terminate for this client, not others
		}
		else {
			printf("Server bytes received: %ld\n", recvStatus);
		}

		// echo back the data received to the client 
		int sendStatus = send(ClientSocket, buffer, recvStatus, 0);
		if (sendStatus == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			return; // TODO ideally retry transmission
		}
		else {
			printf("Server bytes sent: %ld\n", sendStatus);
		}
	}*/
	while (true) { // TODO
	// receive data from client
		int recvStatus = recv(ClientSocket, network_data, DEFAULT_BUFLEN, 0);
		// recvStatus will be the actual length of data recieve instead of buffer size
		if (recvStatus == SOCKET_ERROR) {
			printf("recv failed: %d\n", WSAGetLastError());
			continue;
		}
		else if (recvStatus == 0) {
			printf("Connection closed\n");
			closesocket(ClientSocket);
			return; // TODO only terminate for this client, not others
		}

		printf("Server bytes received: %ld\n", recvStatus);
		ClientPacket cpacket;
		int i = 0;
		while (i < (unsigned int) recvStatus) {
			cpacket.deserialize(&(network_data[i]));
			i += sizeof(ClientPacket);
			printf("Server recieve move: %d", cpacket.move);

			//TODO: do stuff with the packet recieve, now just send the packet back; 
			ServerPacket spacket;
			spacket.move = cpacket.move;
			spacket.valid = true;
			char packet_data[sizeof(ServerPacket)];
			spacket.serialize(packet_data);
			int sendStatus = send(ClientSocket, packet_data, sizeof(ServerPacket), 0);
			if (sendStatus == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				return; // TODO ideally retry transmission
			}
			else {
				printf("Server bytes sent: %ld\n", sendStatus);
			}
		}
	}
}

