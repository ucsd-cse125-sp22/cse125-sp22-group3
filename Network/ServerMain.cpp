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
	
	Plot plotRed{WORLD_PLOT_RED};
	Plot plotBlue{ WORLD_PLOT_BLUE };
	Plot plotYellow{ WORLD_PLOT_YELLOW };
	Plot plotGreen{ WORLD_PLOT_GREEN };
	
	
	Seed seedCarrot{ VegetableType::CARROT, WORLD_SEED_CARROT };
	Seed seedCabbage{ VegetableType::CABBAGE, WORLD_SEED_CABBAGE };
	Seed seedCorn{ VegetableType::CORN, WORLD_SEED_CORN };
	Seed seedTomato{ VegetableType::TOMATO, WORLD_SEED_TOMATO };
	Seed seedRadish{ VegetableType::RADISH, WORLD_SEED_RADISH };


	GameManager game({ &swainky }, { &cabbage, &corn, &radish}, { &plotGreen, &plotYellow, &plotBlue, &plotRed}, { &seedCorn,&seedCabbage ,&seedTomato ,&seedRadish,&seedCarrot});

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