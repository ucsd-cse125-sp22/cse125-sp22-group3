#include "Server.h"

#include <stdio.h>
#include <stdlib.h>
#include <chrono>

// Networking libraries
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Iphlpapi.lib")

Server::Server(void)
{
	// initialize Winsock 2.2
	WSADATA wsaData;
	int startupStatus = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupStatus != 0) {
		fprintf(stderr, "WSAStartup failed: %d\n", startupStatus);
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
		fprintf(stderr, "getaddrinfo failed with error: %d\n", getaddrStatus);
		WSACleanup();
		exit(1);
	}

	// Create a passive SOCKET for the server
	ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		fprintf(stderr, "socket failed with error: %ld\n", WSAGetLastError());
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
			fprintf(stderr, "ioctlsocket failed with error :%d/n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			exit(1);
		}
	}

	// bind passive socket to the local address
	int bindStatus = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (bindStatus == SOCKET_ERROR) {
		fprintf(stderr, "bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	// no longer need address information
	freeaddrinfo(result);

	fprintf(stderr, "Server done initializing.\n");

	// start listening for new clients attempting to connect
	int listenStatus = listen(ListenSocket, SOMAXCONN);
	if (listenStatus == SOCKET_ERROR) {
		fprintf(stderr, "listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	this->printActiveAdapterAddresses();

	fprintf(stderr, "\n");
}

void Server::printActiveAdapterAddresses() {
	ULONG outBufLen = 15000;
	PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	if (pAddresses == NULL) {
		return;
	}

	ULONG family = AF_INET;
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
	DWORD dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

	if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
		free(pAddresses);
		pAddresses = NULL;
		return;
	}

	fprintf(stderr, "Now listening for client connection at:\n");
	if (dwRetVal == NO_ERROR) {
		for (PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next) {
			for (PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress; pUnicast != NULL; pUnicast = pUnicast->Next) {
				if (pCurrAddresses->OperStatus == IfOperStatusUp) {
					sockaddr_in* temp_addr = (sockaddr_in*)(pUnicast->Address.lpSockaddr);
					char s[INET6_ADDRSTRLEN];
					inet_ntop(temp_addr->sin_family, &(temp_addr->sin_addr), s, INET6_ADDRSTRLEN);
					fprintf(stderr, "\t%wS: %s\n", pCurrAddresses->FriendlyName, s);
				}
			}
		}
	}

	if (pAddresses) {
		free(pAddresses);
	}
}

Server::~Server(void)
{
	// shutdown the send half of the connection since no more data will be sent
	for (SOCKET ClientSocket : ClientSocketVec) {
		int shutdownCode = shutdown(ClientSocket, SD_SEND);
		if (shutdownCode == SOCKET_ERROR) {
			fprintf(stderr, "shutdown failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
		}

		// cleanup
		closesocket(ClientSocket);
		WSACleanup();
	}
}

//auto begin_time = std::chrono::steady_clock::now();
void Server::mainLoop(std::function<std::vector<std::pair<char*, int>>(std::vector<ClientPacket> client_packet_vec)> main_code)
{
	fprintf(stderr, "Expecting %d clients\n", NUM_CLIENTS);
	for (int client_idx = 0; client_idx < NUM_CLIENTS; client_idx++) {
		SOCKET client_socket = INVALID_SOCKET;
		client_socket = accept(ListenSocket, NULL, NULL);
		if (client_socket == INVALID_SOCKET) {
			fprintf(stderr, "accept failed: %d\n", WSAGetLastError());
			return; // TODO error condition for this
		}

		struct sockaddr_in peer_addr;
		socklen_t socklen = sizeof(peer_addr);
		getpeername(client_socket, reinterpret_cast<struct sockaddr*>(&peer_addr), &socklen);

		char peer_addr_buf[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &peer_addr.sin_addr, peer_addr_buf, sizeof(peer_addr_buf));

		fprintf(stderr, "\tAccepted client connection from %s:%d (%d/%d)\n", peer_addr_buf, peer_addr.sin_port, client_idx + 1, NUM_CLIENTS);

		// disable Nagle on the client socket
		char value = 1;
		setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		ClientSocketVec.push_back(client_socket);

		// TODO send an message to client_socket indicate the number of other client joined
		ClientWaitPacket cw_packet;
		cw_packet.client_joined = ClientSocketVec.size();
		cw_packet.max_client = NUM_CLIENTS;

		char wait_packet_data[sizeof(ClientWaitPacket)];
		cw_packet.serializeTo(wait_packet_data);
		for (SOCKET temp_sock : ClientSocketVec) {
			int sendStatus = send(temp_sock, wait_packet_data, sizeof(ClientWaitPacket), 0);
			// TODO deal with send_status;
		}
	}
	fprintf(stderr, "All clients connected, starting game loop\n");

main_loop_label:
	while (true) {
		// receive data from clients
		std::vector<ClientPacket> client_packet_vec;
		for (int client_idx = 0; client_idx < NUM_CLIENTS && ClientSocketVec[client_idx] != INVALID_SOCKET; client_idx++) {
			char* total_recv_buf = NULL;
			size_t total_recv_len = 0;
			size_t curr_recv_pos = 0;
			do {
				char network_data[DEFAULT_BUFLEN];
				int recvStatus = recv(ClientSocketVec[client_idx], network_data, DEFAULT_BUFLEN, 0);
				if (recvStatus == SOCKET_ERROR) {
					fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
					goto main_loop_label;
				}
				else if (recvStatus == 0) {
					fprintf(stderr, "Connection closed\n");
					closesocket(ClientSocketVec[client_idx]);
					ClientSocketVec[client_idx] = INVALID_SOCKET;
					break;
				}

				//fprintf(stderr, "Server bytes received: %ld\n", recvStatus);

				if (total_recv_buf == NULL) {
					total_recv_len = *reinterpret_cast<size_t*>(network_data);
					total_recv_buf = static_cast<char*>(malloc(total_recv_len));
					memcpy(total_recv_buf, network_data + sizeof(size_t), recvStatus - sizeof(size_t));
					curr_recv_pos += recvStatus - sizeof(size_t);
				}
				else {
					memcpy(total_recv_buf + curr_recv_pos, network_data, recvStatus);
					curr_recv_pos += recvStatus;
				}
			} while (curr_recv_pos < total_recv_len);

			if (total_recv_buf != NULL) {
				ClientPacket cpacket;
				cpacket.deserializeFrom(total_recv_buf);
				free(total_recv_buf);
				client_packet_vec.push_back(cpacket);
			}
		}
		
		// calls passed-in code
		std::vector<std::pair<char*, int>> out_vec = main_code(client_packet_vec);

		for (int client_idx = 0; client_idx < NUM_CLIENTS; client_idx++) {
			// prepending buffer length in front
			std::pair<char*, int> out_data = out_vec[client_idx];
			size_t send_len = out_data.second;
			char* temp_out_buf = static_cast<char*>(malloc(sizeof(size_t) + send_len));
			memcpy(temp_out_buf, &send_len, sizeof(size_t));
			memcpy(temp_out_buf + sizeof(size_t), out_data.first, send_len);

			int sendStatus = send(ClientSocketVec[client_idx], temp_out_buf, sizeof(size_t) + send_len, 0);
			free(temp_out_buf);
			if (sendStatus == SOCKET_ERROR) {
				fprintf(stderr, "send failed: %d\n", WSAGetLastError());
				return; // TODO ideally retry transmission
			}
			else {
				//fprintf(stderr, "Server bytes sent: %ld\n", sendStatus);
			}

			free(out_data.first);

			//auto end_time = std::chrono::steady_clock::now();
			//long long elapsed_time_ms = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time).count();
			//fprintf(stderr, "Server time between ticks: %lld us\n", elapsed_time_ms);
			//begin_time = end_time;
		}	
	}
}
