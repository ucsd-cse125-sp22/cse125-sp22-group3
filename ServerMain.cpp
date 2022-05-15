#include "Network/Server.h"
#include "GameManager.h"

inline int ServerMain()
{
	Server* server = new Server();
	Player pogo{ CHAR_POGO };
	Player bumbus{ CHAR_BUMBUS };
	Player gilman{ CHAR_GILMAN };
	Player swainky{ CHAR_SWAINKY };

	bumbus.SetWorldPosition({ -20, 30, 0 });
	pogo.SetWorldPosition({ 20, 30, 0 });
	swainky.SetWorldPosition({ 0, 30, -20 });
	gilman.SetWorldPosition({ 0, 30, 20 });
	std::vector<Player*> all_players = { &bumbus, &pogo, &swainky, &gilman };


	std::vector<Player*> players(all_players.begin(), all_players.begin() + server->num_clients);
	GameManager game(players);

	NPC fish{ CHAR_NPC };
	fish.SetWorldPosition({ 0,30, 0 });

	Plot plotRed{ WORLD_PLOT_RED };
	Plot plotBlue{ WORLD_PLOT_BLUE };
	Plot plotYellow{ WORLD_PLOT_YELLOW };
	Plot plotGreen{ WORLD_PLOT_GREEN };

	plotRed.SetPosition({ 100,-4, 100 });
	plotBlue.SetPosition({ -100,-4, -100 });
	plotYellow.SetPosition({ -100,-4, 100 });
	plotGreen.SetPosition({ 100,-4, -100 });

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
	World water{ WORLD_WATER };
	World leaves{ WORLD_LEAVES };
	
	Particle glow{ PARTICLE_GLOW };
	glow.modelAnim = PARTICLE_PLAY;
	glow.SetPosition({ -30, glow.glowParticleHeight, 0 });
	
	game.AddEntities({ &plotRed, &plotBlue, &plotGreen, &plotYellow, &world, &water, &leaves, &fish, &glow });

	Shovel shovel{};
	shovel.SetPosition({ 10, -4, 10 });
	game.AddEntities({ &shovel });

	Hoe hoe{};
	hoe.SetPosition({ 10, -4, -10 });
	game.AddEntities({ &hoe });

	Poison poisson{};
	poisson.SetPosition({ -10, -4, -10 });
	game.AddEntities({ &poisson });

	VeggieNet net{};
	net.SetPosition({ 20, -4, 10 });
	game.AddEntities({ &net });

	WateringCan can{};
	can.SetPosition({ 20, -4, 20 });
	game.AddEntities({ &can });

	Fertilizer fert{};
	fert.SetPosition({ 10, -4, 20 });
	game.AddEntities({ &fert });

	Glue glue{};
	glue.SetPosition({ 10, -1, 30 });
	game.AddEntities({ &glue });

	world.SetPosition({ 0, 0, -4.0f });
	water.SetPosition({ 0, 0, -4.0f });
	leaves.SetPosition({ 0, 0, -4.0f });

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