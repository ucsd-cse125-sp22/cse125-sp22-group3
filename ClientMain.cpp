#include "Client.h"
#include "Client.cpp"

#include <stdio.h>

int main(int argc, char* argv[]) {
	Client* client = new Client("127.0.0.1", "8686");

	const char* dummy_data = "";

	int counter = 0;
	int status;
	do {
		char* dummy_data;
		sprintf(dummy_data, "Hello from the networking team! Iteration: %d", counter++);
		status = client->syncWithServer(dummy_data, strlen(dummy_data) + 1, [](const char* recv_buf, size_t recv_len) {
			printf("Callback echo: %.*s\n", (unsigned int)recv_len, recv_buf);
		});
	} while (status > 0);
	
	delete client;

	return 0;
}