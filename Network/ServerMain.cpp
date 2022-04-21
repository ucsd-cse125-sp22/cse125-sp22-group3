#include "Server.h"
#include "Server.cpp"
#include "../GameManager.h"

int main(int argc, char* argv[]) {
	Server* server = new Server();
	GameManager game{};
	GameManager *gamePtr = &game;
	server->mainLoop([gamePtr](ClientPacket cpacket) {
		if (cpacket.justMoved)
		{
			gamePtr->SetPlayerInput(cpacket.movement, cpacket.player_index);
			if (cpacket.lastCommand == InputCommands::USE)
			{
				gamePtr->SetPlayerUse(cpacket.player_index);
			}
			else if (cpacket.lastCommand == InputCommands::DROP)
			{
				gamePtr->SetPlayerDrop(cpacket.player_index);
			}
		}

		return gamePtr->GetServerBuf();
	});
	//delete server;

	/*
	char out_buf[BUFSIZ];
	memcpy(out_buf, head_struct, size);
	memcpy(out_buf + sizeof(struct ServerPacket), arr, arr_size);
	

	char *server_buf;

	// below for client
	struct ServerPacket *server_str = (cast)server_buf;
	int num_models = server_str->num_models;
	struct ModelInfo *arr = server_buf + sizeof(struct ServerPacket);
	for (int i = 0; i < num_models; i++) {
		arr[i];
	}
*/
	return 0;
}