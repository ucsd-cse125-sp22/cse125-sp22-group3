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
	//NPC fish{ WORLD_PLOT_RED }; // TODO: Ask Cynthia how to import default animations

	/**
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
	*/
	
	Plot plotRed{WORLD_PLOT_RED};
	Plot plotBlue{ WORLD_PLOT_BLUE };
	Plot plotYellow{ WORLD_PLOT_YELLOW };
	Plot plotGreen{ WORLD_PLOT_GREEN };
	
	/**
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
	*/

	World world{ WORLD_MAP };
	World can{ WATERING_CAN };
	Particle glow{ PARTICLE_GLOW };

	//Plot plot{WORLD_PLOT};
	plotRed.SetPosition({ 100,-4, 100});
	plotBlue.SetPosition({ -100,-4, -100});
	plotYellow.SetPosition({ -100,-4, 100});
	plotGreen.SetPosition({ 100,-4, -100});
	fish.SetWorldPosition({ 0,30, 0 });
	can.SetPosition({ 0, -4, -10 });
	glow.SetPosition({ 0, -4, -10 });

	bumbus.SetWorldPosition({ -20, 30, 0 });
	pogo.SetWorldPosition({ 20, 30, 0 });
	swainky.SetWorldPosition({ 0, 30, -20 });
	gilman.SetWorldPosition({ 0, 30, 20 });
	std::vector<Player*> all_players = { &bumbus, &pogo, &swainky, &gilman };

	std::vector<Player*> players(all_players.begin(), all_players.begin() + server->num_clients);
	GameManager game(players);

	game.AddEntities({ &plotRed, &plotBlue, &plotGreen, &plotYellow, &world, &fish, &glow, &can});

	Shovel shovel{};
	shovel.SetPosition({10, -4, 10});
	game.AddEntities({ &shovel });

	Hoe hoe{};
	hoe.SetPosition({ 10, -4, -10 });
	game.AddEntities({ &hoe });

	world.SetPosition({ 0, 0, -4.0f });

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
					case InputCommands::CLOSE_UI:
						game.SetClosePlayerUI(cpacket.player_idx);
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