#include "Server.h"
#include "../ConfigReader.h"

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <string>
#include <set>

// Networking libraries
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Iphlpapi.lib")

Server::Server(void)
{
	static std::unordered_map<std::string, std::string> server_config = ConfigReader::readConfigFile("server.cfg");
	num_clients = stoi(server_config["num_clients"]);

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
	}
	WSACleanup();
}

std::string Server::getRemoteAddressString(SOCKET remote_socket) {
	struct sockaddr_in peer_addr;
	socklen_t socklen = sizeof(peer_addr);
	getpeername(remote_socket, reinterpret_cast<struct sockaddr*>(&peer_addr), &socklen);

	char peer_addr_buf[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &peer_addr.sin_addr, peer_addr_buf, sizeof(peer_addr_buf));

	return std::string(peer_addr_buf).append(":").append(std::to_string(peer_addr.sin_port));
}

std::string Server::getCharacterFriendlyName(ModelEnum model)
{
	switch (model)
	{
	case CHAR_BUMBUS:
		return "Bumbus";
	case CHAR_POGO:
		return "Pogo";
	case CHAR_SWAINKY:
		return "Swainky";
	case CHAR_GILMAN:
		return "Gilman";
	default:
		return "Not a character";
	}
}

//auto begin_time = std::chrono::steady_clock::now();
void Server::mainLoop(std::function<std::vector<std::pair<char*, int>>(std::vector<ClientPacket> client_packet_vec)> main_code)
{
	fprintf(stderr, "Expecting %d clients\n", num_clients);
	for (int client_idx = 0; client_idx < num_clients; client_idx++) {
begin_loop_accept_client:
		SOCKET client_socket = INVALID_SOCKET;
		client_socket = accept(ListenSocket, NULL, NULL);
		if (client_socket == INVALID_SOCKET) {
			fprintf(stderr, "Failed to accept new client connection, error code: %d\n", WSAGetLastError());

			if (WSAGetLastError() == WSAECONNRESET) {
				closesocket(client_socket);
				goto begin_loop_accept_client;
			}
			else {
				Server::~Server(); // gracefully shut down
				exit(1);
			}
		}

		std::string client_addr_str = getRemoteAddressString(client_socket);
		fprintf(stderr, "\tAccepted client connection from %s (%d/%d)\n", client_addr_str.c_str(), client_idx + 1, num_clients);

		// disable Nagle on the client socket
		char value = 1;
		setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		ClientSocketVec.push_back(client_socket);

		// send a message to connected clients, updating about join status
		ClientWaitPacket cw_packet;
		cw_packet.client_joined = ClientSocketVec.size();
		cw_packet.max_client = num_clients;

		char wait_packet_data[sizeof(ClientWaitPacket)];
		cw_packet.serializeTo(wait_packet_data);
		for (SOCKET temp_sock : ClientSocketVec) {
			int sendStatus = send(temp_sock, wait_packet_data, sizeof(ClientWaitPacket), 0);
			// TODO deal with send_status;
		}
	}
	fprintf(stderr, "All clients connected, starting game loop\n");

	// BEGIN character Selection
	ServerCharacterPacket sc_packet = {};
	sc_packet.char_options[0] = -1;
	sc_packet.char_options[1] = -1;
	sc_packet.char_options[2] = -1;
	sc_packet.char_options[3] = -1;
	sc_packet.ready = false; 

	ClientCharacterPacket cc_packet = {};
	std::set<int> char_unavailable; 
	int char_selections[4] = { -1,-1,-1,-1 };	//TODO: hardcoded! need to change to dynamic allocation
	int num_client_selected = 0; 

	while (num_client_selected <= num_clients) {
		//send all character option to clients
		sc_packet.ready = num_client_selected == num_clients; 
		char sc_packet_data[sizeof(ServerCharacterPacket)];
		for (int client_idx = 0; client_idx < num_clients; client_idx++) {
			sc_packet.my_char_index = client_idx;
			sc_packet.serializeTo(sc_packet_data);
			int sendStatus = send(ClientSocketVec[client_idx], sc_packet_data, sizeof(ServerCharacterPacket), 0);

			// TODO deal with send_status;
		}
		if (num_client_selected == num_clients) break; 

		// recieve character selection result from clients
		// TODO: this structure definitely gonna cause race condition.... need somelike of breaktie things
		for (int client_idx = 0; client_idx < num_clients; client_idx++) {
			char cc_packet_data[sizeof(ClientCharacterPacket)];
			int recvStatus = recv(ClientSocketVec[client_idx], cc_packet_data, sizeof(cc_packet_data), 0);

			if (recvStatus == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAECONNRESET) {
					std::string client_addr_str = getRemoteAddressString(ClientSocketVec[client_idx]);
					fprintf(stderr, "Connection with client #%d at %s dropped\n", client_idx, client_addr_str.c_str());

					closesocket(ClientSocketVec[client_idx]);
					ClientSocketVec[client_idx] = INVALID_SOCKET;
				}
				else {
					fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
				}
				break; // break out of the recv do-while loop
			}
			cc_packet.deserializeFrom(cc_packet_data);

			if (cc_packet.confirm_selection && char_selections[client_idx]<0 ) {
				const bool avaliable = char_unavailable.find(cc_packet.character) == char_unavailable.end();
				if (avaliable) {
					printf("recv:%d %s \n", cc_packet.character, cc_packet.confirm_selection ? "true" : "false");
					printf("Avaliable! value set! \n");
					char_selections[client_idx] = cc_packet.character;
					char_unavailable.insert(cc_packet.character);
					sc_packet.char_options[client_idx] = cc_packet.character;
					num_client_selected++;
				}
			}
		}
	}
	fprintf(stderr, "All clients Character Selected, starting game loop\n");


main_loop_label:
	while (true) {
		// receive data from clients
		std::vector<ClientPacket> client_packet_vec;
		for (int client_idx = 0; client_idx < num_clients; client_idx++) {
			if (ClientSocketVec[client_idx] == INVALID_SOCKET) {
				continue;
			}

			char* total_recv_buf = NULL;
			size_t total_recv_len = 0;
			size_t curr_recv_pos = 0;
			do {
				char network_data[DEFAULT_BUFLEN];
				int recvStatus = recv(ClientSocketVec[client_idx], network_data, DEFAULT_BUFLEN, 0);
				if (recvStatus == SOCKET_ERROR) {
					if (WSAGetLastError() == WSAECONNRESET) {
						std::string client_addr_str = getRemoteAddressString(ClientSocketVec[client_idx]);
						fprintf(stderr, "Connection with client #%d at %s dropped\n", client_idx, client_addr_str.c_str());

						closesocket(ClientSocketVec[client_idx]);
						ClientSocketVec[client_idx] = INVALID_SOCKET;
					}
					else {
						fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
					}

					break; // break out of the recv do-while loop
				}
				else if (recvStatus == 0) {
					std::string client_addr_str = getRemoteAddressString(ClientSocketVec[client_idx]);
					fprintf(stderr, "Client #%d at %s closed the connection\n", client_idx, client_addr_str.c_str());

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
				cpacket.player_idx = client_idx;
				client_packet_vec.push_back(cpacket);
			}
		}
		
		// calls passed-in code
		std::vector<std::pair<char*, int>> out_vec = main_code(client_packet_vec);

		for (int client_idx = 0; client_idx < num_clients; client_idx++) {
			if (ClientSocketVec[client_idx] == INVALID_SOCKET) {
				free(out_vec[client_idx].first);
				continue;
			}

			// prepending buffer length in front
			std::pair<char*, int> out_data = out_vec[client_idx];
			size_t send_len = out_data.second;
			char* temp_out_buf = static_cast<char*>(malloc(sizeof(size_t) + send_len));
			memcpy(temp_out_buf, &send_len, sizeof(size_t));
			memcpy(temp_out_buf + sizeof(size_t), out_data.first, send_len);

			int sendStatus = send(ClientSocketVec[client_idx], temp_out_buf, sizeof(size_t) + send_len, 0);
			free(temp_out_buf);
			if (sendStatus == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAECONNRESET) {
					std::string client_addr_str = getRemoteAddressString(ClientSocketVec[client_idx]);
					fprintf(stderr, "Connection with client #%d at %s dropped\n", client_idx, client_addr_str.c_str());

					closesocket(ClientSocketVec[client_idx]);
					ClientSocketVec[client_idx] = INVALID_SOCKET;
				}
				else {
					fprintf(stderr, "send failed: %d\n", WSAGetLastError());
				}
				// TODO current behavior: skip sending if failed
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
