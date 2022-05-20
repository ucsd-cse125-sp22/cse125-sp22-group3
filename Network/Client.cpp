#include "Client.h"
#include "NetworkPacket.h"
#include <stdio.h>
#include <stdlib.h>
#include <functional>

// Networking libraries
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

Client::Client(const char* server_addr, const char* server_port)
{
	// initialize Winsock 2.2
	WSADATA wsaData;
	int startupStatus = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupStatus != 0) {
		fprintf(stderr, "WSAStartup failed: %d\n", startupStatus);
		exit(1);
	}

	// hints for request to get server address
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// resolve server address and port info
	fprintf(stderr, "Connecting to server at %s:%s\n", server_addr, server_port);
	struct addrinfo* result = NULL;
	int getaddrStatus = getaddrinfo(server_addr, server_port, &hints, &result);
	if (getaddrStatus != 0) { // can't resolve server hostname / IP address
		fprintf(stderr, "getaddrinfo failed: %d\n", getaddrStatus);
		WSACleanup();
		exit(1);
	}

	// attempt all addresses from getaddrinfo's result
	for (struct addrinfo* addr = result; addr != NULL; addr = addr->ai_next) {
		// creating a socket for the client conforming to current address's protocols
		ConnectSocket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			fprintf(stderr, "socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			continue;
		}

		// connect to the current address
		int connectStatus = connect(ConnectSocket, addr->ai_addr, (int)addr->ai_addrlen);
		if (connectStatus == SOCKET_ERROR) {
			fprintf(stderr, "connect failed with error: %ld\n", WSAGetLastError());
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
		fprintf(stderr, "Unable to connect to server!\n");
		WSACleanup();
		exit(1);
	}

	bool isNonblocking = false; // TODO revisit later, depending on game architecture
	if (isNonblocking) {
		// set the mode of the socket to be nonblocking
		u_long iMode = 1;
		int ioctlResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
		if (ioctlResult == SOCKET_ERROR) {
			fprintf(stderr, "ioctlsocket failed with error :%d/n", WSAGetLastError());
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
		fprintf(stderr, "shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	// TODO deal with remaining data, before server also closes connection

	closesocket(ConnectSocket);
	WSACleanup();
}

void Client::syncGameReadyToStart(std::function<void(ClientWaitPacket in_wait_packet)> callback) {
	bool wait_for_clients = true;
	do {
		char recvbuf[DEFAULT_BUFLEN];
		int recvStatus = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (recvStatus == SOCKET_ERROR) {
			fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
		}
		else if (recvStatus == 0) {
			fprintf(stderr, "Connection closed by server\n");
		}

		for (int byte_idx = 0; byte_idx < recvStatus; byte_idx += sizeof(ClientWaitPacket)) {
			ClientWaitPacket cw_packet;
			cw_packet.deserializeFrom(recvbuf + byte_idx);
			callback(cw_packet);

			wait_for_clients = cw_packet.client_joined < cw_packet.max_client;
		}
	} while (wait_for_clients);
}

void Client::syncCharacterSelection(std::function<void(ServerCharacterPacket recv_packet)> callback)
{
	boolean done = false; 
	do {
		char recvbuf[DEFAULT_BUFLEN];
		int recvStatus = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (recvStatus == SOCKET_ERROR) {
			fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
		}
		else if (recvStatus == 0) {
			fprintf(stderr, "Connection closed by server\n");
		}

		for (int byte_idx = 0; byte_idx < recvStatus; byte_idx += sizeof(ServerCharacterPacket)) {
			ServerCharacterPacket sc_packet;
			sc_packet.deserializeFrom(recvbuf + byte_idx);
			done = sc_packet.ready; 
			callback(sc_packet);
		}
	} while (!done);
}

/*
 * Returns bytes received from server. If return value is 0, server has closed connection.
 * Delete the client object immediately.
 */
int Client::syncWithServer(const void* send_buf, size_t send_len,
	std::function<void(char* recv_buf, size_t recv_len)> callback)
{
	// prepending buffer length in front
	char* temp_out_buf = static_cast<char*>(malloc(sizeof(size_t) + send_len));
	memcpy(temp_out_buf, &send_len, sizeof(size_t));
	memcpy(temp_out_buf + sizeof(size_t), send_buf, send_len);

	// sending data (inputs)
	int sendStatus = send(ConnectSocket, temp_out_buf, sizeof(size_t) + send_len, 0);
	free(temp_out_buf);
	if (sendStatus == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAECONNRESET) {
			fprintf(stderr, "Connection with server dropped\n");
		}
		else {
			fprintf(stderr, "send failed: %d\n", WSAGetLastError());
		}
		Client::~Client();
		return 0;
	}
	else {
		//fprintf(stderr, "Client bytes sent: %ld\n", sendStatus);
	}

	//receiving data (updated state)
	char* total_recv_buf = NULL;
	size_t total_recv_len = 0;
	size_t curr_recv_pos = 0;
	do {
		char recvbuf[DEFAULT_BUFLEN];
		int recvStatus = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (recvStatus == SOCKET_ERROR) {
			fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
			return 0;
		}
		else if (recvStatus == 0) {
			fprintf(stderr, "Connection closed by server\n");
			return 0;
		}
		
		//fprintf(stderr, "Client bytes received: %ld\n", recvStatus);

		if (total_recv_buf == NULL) {
			total_recv_len = *reinterpret_cast<size_t*>(recvbuf);
			total_recv_buf = static_cast<char*>(malloc(total_recv_len));
			memcpy(total_recv_buf, recvbuf + sizeof(size_t), recvStatus - sizeof(size_t));
			curr_recv_pos += recvStatus - sizeof(size_t);
		}
		else {
			memcpy(total_recv_buf + curr_recv_pos, recvbuf, recvStatus);
			curr_recv_pos += recvStatus;
		}
	} while (curr_recv_pos < total_recv_len);
	
	callback(total_recv_buf, total_recv_len);

	if (total_recv_buf != NULL) {
		free(total_recv_buf);
	}

	return total_recv_len;
}

