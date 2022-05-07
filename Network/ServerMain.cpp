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


	NPC fish{ CHAR_NPC };

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

	
	Plot plotRed{WORLD_PLOT_RED};
	Plot plotBlue{ WORLD_PLOT_BLUE };
	Plot plotYellow{ WORLD_PLOT_YELLOW };
	Plot plotGreen{ WORLD_PLOT_GREEN };
	
	
	Seed seedCarrot{ VegetableType::CARROT, WORLD_SEED_CARROT };
	Seed seedCabbage{ VegetableType::CABBAGE, WORLD_SEED_CABBAGE };
	Seed seedCorn{ VegetableType::CORN, WORLD_SEED_CORN };
	Seed seedTomato{ VegetableType::TOMATO, WORLD_SEED_TOMATO };
	Seed seedRadish{ VegetableType::RADISH, WORLD_SEED_RADISH };

	Seed flagCarrot{ VegetableType::CARROT, WORLD_FLAG_CARROT };
	Seed flagCabbage{ VegetableType::CABBAGE, WORLD_FLAG_CABBAGE };
	Seed flagCorn{ VegetableType::CORN, WORLD_FLAG_CORN };
	Seed flagTomato{ VegetableType::TOMATO, WORLD_FLAG_TOMATO };
	Seed flagRadish{ VegetableType::RADISH, WORLD_FLAG_RADISH };

	World world{ WORLD_MAP };

	//GameManager game({ &swainky }, { &cabbage, &corn, &radish, &carrot, &tomato}, { &plotGreen, &plotYellow, &plotBlue, &plotRed}, { &seedCorn,&seedCabbage ,&seedTomato ,&seedRadish,&seedCarrot});

	tomato.SetPosition({i++ * 15, 5, 0});

	//Plot plot{WORLD_PLOT};
	plotRed.SetPosition({ 80,30,0 });
	
	//Seed seed{ VegetableType::CABBAGE, WORLD_SEED };
	seedCorn.SetPosition({ 100, 30, 0 });
	
	fish.SetWorldPosition({ 0,30,0 });
	GameManager game({ &pogo,&bumbus, &gilman, &swainky });
	game.AddEntities({ &cabbage, &corn, &radish, &plotRed, &seedTomato, &seedCorn, &world});

	world.SetPosition({ 0, 0, -5.0f });


	server->mainLoop([&game](std::vector<ClientPacket> client_packet_vec) {
		for (ClientPacket& cpacket : client_packet_vec) {
			if (cpacket.justMoved)
			{
				game.SetPlayerInput(cpacket.movement, cpacket.player_idx);
				switch (cpacket.lastCommand) {
					case InputCommands::USE:
						game.SetPlayerUse(cpacket.player_idx);
						break;
					case InputCommands::DROP:
						game.SetPlayerDrop(cpacket.player_idx);
						break;
					case InputCommands::SPRINT:
						game.SetPlayerSprint(cpacket.player_idx, true);
						break;
					case InputCommands::STOP_SPRINT:
						game.SetPlayerSprint(cpacket.player_idx, false);
						break;
					case InputCommands::DANCE_CMD:
						game.SetPlayerDance(cpacket.player_idx);
						break;
					case InputCommands::SELL_CMD:
						game.SetPlayerSell(cpacket.player_idx);
						break;
					case InputCommands::BUY_CABBAGE:
						game.SetPlayerBuy(cpacket.player_idx, VegetableType::CABBAGE); // Is there a nicer way of doing this?
						break;
					case InputCommands::BUY_CARROT:
						game.SetPlayerBuy(cpacket.player_idx, VegetableType::CARROT);
						break;
					case InputCommands::BUY_CORN:
						game.SetPlayerBuy(cpacket.player_idx, VegetableType::CORN);
						break;
					case InputCommands::BUY_TOMATO:
						game.SetPlayerBuy(cpacket.player_idx, VegetableType::TOMATO); 
						break;
					case InputCommands::BUY_RADISH:
						game.SetPlayerBuy(cpacket.player_idx, VegetableType::RADISH); 
						break;
					default: break;
				}
			}
		}

		game.FixedUpdate();
		
		return game.GetServerBuf();
	});
	//delete server;
	
	return 0;
}