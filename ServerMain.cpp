#include "Network/Server.h"
#include "GameManager.h"

inline int ServerMain()
{
	Server* server = new Server();

	server->WaitForClients();
	std::vector<ModelEnum> character_selection = server->CharacterSelection();

	Player bumbus{CHAR_BUMBUS};
	Player pogo{CHAR_POGO};
	Player swainky{CHAR_SWAINKY};
	Player gilman{CHAR_GILMAN};

	bumbus.SetWorldPosition({-20, 30, 0});
	pogo.SetWorldPosition({20, 30, 0});
	swainky.SetWorldPosition({0, 30, -20});
	gilman.SetWorldPosition({0, 30, 20});

	std::unordered_map<ModelEnum, Player*> char_map = {
		{CHAR_BUMBUS, &bumbus},
		{CHAR_POGO, &pogo},
		{CHAR_SWAINKY, &swainky},
		{CHAR_GILMAN, &gilman},
	};

	std::vector<Player*> players;
	for (ModelEnum char_models : character_selection)
	{
		players.push_back(char_map[char_models]);
	}
	
	GameManager game(players);

	NPC nanaue{CHAR_NPC};
	nanaue.SetWorldPosition({0, 30, 0});

	// TODO: make this look better LUL
	Plot plotRed{WORLD_PLOT_RED};
	Plot plotBlue{WORLD_PLOT_BLUE};
	Plot plotYellow{WORLD_PLOT_YELLOW};
	Plot plotGreen{WORLD_PLOT_GREEN};

	plotRed.SetPosition({100, -4, 100});
	plotBlue.SetPosition({-100, -4, -100});
	plotYellow.SetPosition({-100, -4, 100});
	plotGreen.SetPosition({100, -4, -100});

	Plot plotRed1{WORLD_PLOT_RED};
	Plot plotBlue1{WORLD_PLOT_BLUE};
	Plot plotYellow1{WORLD_PLOT_YELLOW};
	Plot plotGreen1{WORLD_PLOT_GREEN};

	plotRed1.SetPosition({100, -4, 90});
	plotBlue1.SetPosition({-100, -4, -90});
	plotYellow1.SetPosition({-100, -4, 90});
	plotGreen1.SetPosition({100, -4, -90});

	Plot plotRed2{WORLD_PLOT_RED};
	Plot plotBlue2{WORLD_PLOT_BLUE};
	Plot plotYellow2{WORLD_PLOT_YELLOW};
	Plot plotGreen2{WORLD_PLOT_GREEN};

	plotRed2.SetPosition({90, -4, 100});
	plotBlue2.SetPosition({-90, -4, -100});
	plotYellow2.SetPosition({-90, -4, 100});
	plotGreen2.SetPosition({90, -4, -100});

	Plot plotRed3{WORLD_PLOT_RED};
	Plot plotBlue3{WORLD_PLOT_BLUE};
	Plot plotYellow3{WORLD_PLOT_YELLOW};
	Plot plotGreen3{WORLD_PLOT_GREEN};

	plotRed3.SetPosition({90, -4, 90});
	plotBlue3.SetPosition({-90, -4, -90});
	plotYellow3.SetPosition({-90, -4, 90});
	plotGreen3.SetPosition({90, -4, -90});

	game.AddEntities({
		&plotRed1, &plotRed2, &plotRed3,
		&plotBlue1, &plotBlue2, &plotBlue3,
		&plotGreen1, &plotGreen2, &plotGreen3,
		&plotYellow1, &plotYellow2, &plotYellow3
	});

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

	World world{WORLD_MAP};
	World water{WORLD_WATER};
	World leaves{WORLD_LEAVES};
	World grass{WORLD_GRASS};
	World dome{WORLD_DOME};

	game.AddEntities({&plotRed, &plotBlue, &plotGreen, &plotYellow, &world, &water, &leaves, &nanaue, &grass, &dome});

	WateringCan can{};
	can.SetPosition({15, -3, 0});
	game.AddEntities({&can});

	world.SetPosition({0, 0, -4.0f});
	water.SetPosition({0, 0, -4.0f});
	leaves.SetPosition({0, 0, -4.0f});
	grass.SetPosition({0, 0, -4.0f});
	dome.SetPosition({0, 0, -4.0f});

	server->mainLoop([&game](std::vector<ClientPacket> &client_packet_vec)
	{
		game.StartGameTime();

		for (ClientPacket& cpacket : client_packet_vec)
		{
			game.SetPlayerInput(cpacket.movement, cpacket.player_idx);
			switch (cpacket.lastCommand)
			{
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
				game.SetPlayerBuy(cpacket.player_idx, VegetableType::CABBAGE);
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
				break;
			case InputCommands::BUY_NET:
				game.SetPlayerBuy(cpacket.player_idx, ModelEnum::NET);
				break;
			case InputCommands::BUY_HOE:
				game.SetPlayerBuy(cpacket.player_idx, ModelEnum::HOE);
				break;
			case InputCommands::BUY_WATER:
				game.SetPlayerBuy(cpacket.player_idx, ModelEnum::WATERING_CAN);
				break;
			case InputCommands::BUY_FERTILIZER:
				game.SetPlayerBuy(cpacket.player_idx, ModelEnum::FERTILIZER);
				break;
			case InputCommands::BUY_SHOVEL:
				game.SetPlayerBuy(cpacket.player_idx, ModelEnum::SHOVEL);
				break;
			case InputCommands::BUY_GLUE:
				game.SetPlayerBuy(cpacket.player_idx, ModelEnum::GLUE);
				break;
			case InputCommands::BUY_POISON:
				game.SetPlayerBuy(cpacket.player_idx, ModelEnum::POISON);
				break;
			case InputCommands::BUY_OATS:
				game.SetPlayerBuy(cpacket.player_idx, ModelEnum::OATS);
				break;
			case InputCommands::BUY_SOJU:
				game.SetPlayerBuy(cpacket.player_idx, ModelEnum::SOJU);
				break;
			default: break;
			}
		}

		game.FixedUpdate();

		return game.GetServerBuf();
	});

	return 0;
}
