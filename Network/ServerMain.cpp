#include "Server.h"
#include "../GameManager.h"

//int main(int argc, char* argv[]) {
inline int ServerMain(){
	Server* server = new Server();
	Player pogo{CHAR_POGO};
	GameManager game({&pogo}, {});
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

		gamePtr->FixedUpdate();
		
		return gamePtr->GetServerBuf();
	});
	//delete server;
	
	return 0;
}