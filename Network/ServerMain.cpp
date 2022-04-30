#include "Server.h"
#include "../GameManager.h"

//int main(int argc, char* argv[]) {
inline int ServerMain()
{
	Server* server = new Server();
	Player pogo{CHAR_POGO};
	Player bumbus{CHAR_BUMBUS};
	Player gilman{CHAR_GILMAN};
	Player swainky{CHAR_SWAINKY};

	int i = 1;
	Vegetable cabbage{ VegetableType::CABBAGE, VEG_CABBAGE };
	cabbage.SetPosition({i++ * 15, 5, 0});
	Vegetable carrot{VegetableType::CARROT, VEG_CARROT};
	carrot.SetPosition({i++ * 15, 5, 0});
	Vegetable corn {VegetableType::CORN, VEG_CORN};
	corn.SetPosition({i++ * 15, 5, 0});
	Vegetable radish{VegetableType::RADISH, VEG_RADISH};
	radish.SetPosition({i++ * 15, 5, 0});
	Vegetable tomato{VegetableType::TOMATO, VEG_TOMATO};
	tomato.SetPosition({i++ * 15, 5, 0});

	Plot plot{WORLD_PLOT};
	plot.SetPosition({ 80,30,0 });
	
	Seed seed{ VegetableType::CABBAGE, WORLD_SEED };
	seed.SetPosition({ 100, 30, 0 });
	
	
	GameManager game({ &swainky });
	game.AddEntities({ &cabbage, &corn, &radish, &plot, &seed });

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