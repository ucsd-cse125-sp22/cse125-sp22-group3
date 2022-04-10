#include "Server.h"
#include "Server.cpp"

int main(int argc, char* argv[]) {
	Server* server = new Server();
	server->mainLoop();
	//delete server;

	return 0;
}