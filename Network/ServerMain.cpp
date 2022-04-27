#include "Server.h"
#include "../GameManager.h"

//int main(int argc, char* argv[]) {
inline int ServerMain(){
	Server* server = new Server();
	Player pogo{CHAR_POGO};
	Player bumbus{CHAR_BUMBUS};
	Player gilman{CHAR_GILMAN};
	Player swainky{CHAR_SWAINKY};
	Vegetable cabbage{ VegetableType::CABBAGE, VEG_CABBAGE };
	Vegetable carrot{VegetableType::CARROT, VEG_CARROT};
	Vegetable corn {VegetableType::CORN, VEG_CORN};
	Vegetable radish{VegetableType::RADISH, VEG_RADISH};
	Vegetable tomato{VegetableType::TOMATO, VEG_TOMATO};
	GameManager game({&pogo, &bumbus, &gilman, &swainky}, {&cabbage, &carrot, &radish, &tomato, &corn}, {});

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