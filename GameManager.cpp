#include "GameManager.h"

#include "Network/NetworkPacket.h"

std::chrono::steady_clock::time_point GameManager::curr_time_ = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point GameManager::last_time_ = std::chrono::steady_clock::now();

GameManager::GameManager()
{
	physics_ = PhysicsEngine();
}
GameManager::GameManager(std::vector<Player*> players, std::vector<Vegetable*> vegetables, std::vector<Plot*> plots)
{
	// Initialize Players
	players_ = players;
	int i = 0;
	for (Player* player : players_) {
		// Set Player Positions
		player->SetWorldPosition({i * 10,0,0});
		// Add Players to Entities list
		game_entities.push_back(player);
		game_entities.back()->type = EntityType::PLAYER;
		i++;
	}

	// Testing vegetables/plots, eventually would want to include a vector of GameEntities instead of separating each?
	vegetables_ = vegetables;
	for (Vegetable* vegetable : vegetables_) {
		vegetable->SetPosition({ i * 15,5,0 });
		game_entities.push_back(vegetable);
		game_entities.back()->type = EntityType::VEGETABLE;
		i++;
	}
	plots_ = plots;
	for (Plot* plot : plots_) {
		plot->SetPosition({ 80,30,0 });
		game_entities.push_back(plot);
		game_entities.back()->type = EntityType::PLOT;
	}

	// Instantiate Physics Engine
	std::vector<PhysicsObject*> physics_objects;
	for (Player* player : players_) {
		physics_objects.push_back(player);
	}
	for (Vegetable* vegetable : vegetables_) {
		physics_objects.push_back(vegetable);
	}
	for (Plot* plot : plots_) {
		physics_objects.push_back(plot);
	}
	physics_ = PhysicsEngine(physics_objects);

}

GameManager::GameManager(std::vector<Player*> players, std::vector<Vegetable*> vegetables, std::vector<Plot*> plots, std::vector<Seed*> seeds)
{
	// Initialize Players
	players_ = players;
	int i = 0;
	for (Player* player : players_) {
		// Set Player Positions
		player->SetWorldPosition({ i * 10,0,0 });
		// Add Players to Entities list
		game_entities.push_back(player);
		game_entities.back()->type = EntityType::PLAYER;
		i++;
	}

	// Testing vegetables/plots, eventually would want to include a vector of GameEntities instead of separating each?
	vegetables_ = vegetables;
	for (Vegetable* vegetable : vegetables_) {
		vegetable->SetPosition({ i * 15,5,0 });
		game_entities.push_back(vegetable);
		game_entities.back()->type = EntityType::VEGETABLE;
		i++;
	}
	plots_ = plots;
	for (Plot* plot : plots_) {
		plot->SetPosition({ 80,30,0 });
		game_entities.push_back(plot);
		game_entities.back()->type = EntityType::PLOT;
	}

	seeds_ = seeds;
	for (Seed* seed : seeds_) {
		seed->SetPosition({ 100, 30, 0 });
		game_entities.push_back(seed);
		game_entities.back()->type = EntityType::SEED;
	}

	// Instantiate Physics Engine
	std::vector<PhysicsObject*> physics_objects;
	for (Player* player : players_) {
		physics_objects.push_back(player);
	}
	for (Vegetable* vegetable : vegetables_) {
		physics_objects.push_back(vegetable);
	}
	for (Plot* plot : plots_) {
		physics_objects.push_back(plot);
	}
	for (Seed* seed : seeds_) {
		physics_objects.push_back(seed);
	}
	physics_ = PhysicsEngine(physics_objects);

}

void GameManager::FixedUpdate()
{
	GameManager::UpdateFixedDeltaTime();
	for (GameEntity* entity : game_entities) {
		entity->FixedUpdate();
	}

	// Check collisions
	physics_.Compute();
}

std::pair<char*, int> GameManager::GetServerBuf()
{
	std::vector<ModelInfo> model_infos;
	for (GameEntity* entity : game_entities) {
		switch (entity->type) {
			case EntityType::PLAYER: {
				auto player = dynamic_cast<Player*>(entity);
				if (player != nullptr) {
					model_infos.push_back(ModelInfo{
						reinterpret_cast<uintptr_t>(player),
						player->GetModel(),
						player->modelAnim,
						player->GetParentTransform()
					});
				}
			}
			
			case EntityType::VEGETABLE: {
				auto vegetable = dynamic_cast<Vegetable*>(entity);
				if (vegetable != nullptr) {
					model_infos.push_back(ModelInfo{
						reinterpret_cast<uintptr_t>(vegetable),
						vegetable->GetModel(),
						vegetable->modelAnim,
						vegetable->GetParentTransform()
						});
				}
			}

			case EntityType::PLOT: {
				auto plot = dynamic_cast<Plot*>(entity);
				if (plot != nullptr) {
					model_infos.push_back(ModelInfo{
						reinterpret_cast<uintptr_t>(plot),
						plot->GetModel(),
						plot->modelAnim,
						plot->GetParentTransform()
						});
				}
			}

			case EntityType::SEED: {
				auto player = dynamic_cast<Seed*>(entity);
				if (player != nullptr) {
					model_infos.push_back(ModelInfo{
						reinterpret_cast<uintptr_t>(player),
						player->GetModel(),
						player->modelAnim,
						player->GetParentTransform()
						});
				}
			}
		}
	}

	//for (Player* player : players_)
	//{
	//	model_infos.push_back(ModelInfo{
	//		reinterpret_cast<uintptr_t>(player),
	//		player->GetModel(),
	//		player->modelAnim,
	//		player->GetParentTransform()
	//	});
	//}

	ServerHeader sheader{};
	sheader.num_models = model_infos.size();
	sheader.player_transform = players_[0]->GetParentTransform(); // TODO assumes player index 0
	
	auto server_buf = static_cast<char*>(malloc(GetBufSize(&sheader)));
	serverSerialize(server_buf, &sheader, model_infos.data());
	return std::make_pair(server_buf, GetBufSize(&sheader));
}

void GameManager::Draw(const glm::mat4 view, const glm::mat4 projection, const GLuint shader)
{
	for (auto entity : game_entities) {
		if (entity->type == EntityType::PLAYER) {
			dynamic_cast<Player*>(entity)->Draw(view, projection, shader);
		}
		else if (entity->type == EntityType::VEGETABLE) {
			dynamic_cast<Vegetable*>(entity)->Draw(view, projection, shader);
		}
		else if (entity->type == EntityType::PLOT) {
			dynamic_cast<Plot*>(entity)->Draw(view, projection, shader);
		}
	}
}

void GameManager::Draw(const GLuint shader)
{
	for (auto entity : game_entities) {
		if (entity->type == EntityType::PLAYER) {
			dynamic_cast<Player*>(entity)->Draw(shader);
		}
		else if (entity->type == EntityType::VEGETABLE) {
			dynamic_cast<Vegetable*>(entity)->Draw(shader);
		}
	}
}

void GameManager::SetPlayerInput(const glm::vec2 move_input, const int player_index)
{
	players_[player_index]->move_input = move_input;
}

//TODO Merge these two with SetPlayerInput when NetworkPacket.h is updated
void GameManager::SetPlayerUse(const int player_index) {
	players_[player_index]->Use();
}

void GameManager::SetPlayerDrop(const int player_index) {
	players_[player_index]->Drop();
}

glm::vec3 GameManager::GetPlayerPosition(const int player_index) const
{
	return players_[player_index]->GetPosition();
}

double GameManager::GetFixedDeltaTime() {
	const std::chrono::duration<double> elapsed_seconds = curr_time_ - last_time_;
    return elapsed_seconds.count();
}

void GameManager::UpdateFixedDeltaTime()
{
	last_time_ = curr_time_;
	curr_time_ = std::chrono::steady_clock::now();
}