#include "Server.h"
#include "../GameManager.h"

//int main(int argc, char* argv[]) {
inline int ServerMain(){
	Server* server = new Server();
	Player pogo{CHAR_POGO};
	Player bumbus{CHAR_BUMBUS};
	Player bumbus_2{CHAR_BUMBUS};
	Player swainky{CHAR_SWAINKY};
	GameManager game({&pogo, &bumbus, &bumbus_2, &swainky}, {});
	server->mainLoop([&game](const ClientPacket cpacket) {
		if (cpacket.justMoved)
		{
			game.SetPlayerInput(cpacket.movement, cpacket.player_index);
			if (cpacket.lastCommand == InputCommands::USE)
			{
				game.SetPlayerUse(cpacket.player_index);
			}
			else if (cpacket.lastCommand == InputCommands::DROP)
			{
				game.SetPlayerDrop(cpacket.player_index);
			}
		}

		game.FixedUpdate();
		
		return game.GetServerBuf();
	});
	//delete server;
	
	return 0;
}