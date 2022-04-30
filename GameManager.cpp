#include "GameManager.h"

#include "Network/NetworkPacket.h"

std::chrono::steady_clock::time_point GameManager::curr_time_ = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point GameManager::last_time_ = std::chrono::steady_clock::now();

GameManager::GameManager()
{
	physics = PhysicsEngine();
}
GameManager::GameManager(std::vector<Player*> players)
{
	// Initialize Players
	players_ = players;
	int i = 0;
	for (Player* player : players_) {
		// Set Player Positions
		player->SetWorldPosition({i * 10,0,0});
		// Add Players to Entities list
		game_entities.push_back(player);
		i++;
	}

	// Instantiate Physics Engine
	std::vector<PhysicsObject*> physics_objects;
	for (Player* player : players_) {
		physics_objects.push_back(player);
	}
	physics = PhysicsEngine(physics_objects);
}

void GameManager::FixedUpdate()
{
	GameManager::UpdateFixedDeltaTime();
	for (GameEntity* entity : game_entities) {
		entity->FixedUpdate();
	}

	// Check collisions
	physics.Compute();
}
void GameManager::AddEntities(std::vector<GameEntity*> entities)
{
	for (GameEntity* entity : entities) {
		game_entities.push_back(entity);
		auto phys_obj = dynamic_cast<PhysicsObject*>(entity);
		if (phys_obj) {
			physics.AddPhysObject(phys_obj);
		}
	}
}

std::pair<char*, int> GameManager::GetServerBuf()
{
	std::vector<ModelInfo> model_infos;
	for (GameEntity* entity : game_entities) {

		auto drawable = dynamic_cast<Drawable*>(entity);
		if (drawable) {
			model_infos.push_back(ModelInfo{
				reinterpret_cast<uintptr_t>(entity),
				drawable->GetModelEnum(),
				drawable->GetAniMode(),
				drawable->GetParentTransform()
			});
		}
	}

	ServerHeader sheader{};
	sheader.num_models = model_infos.size();
	sheader.player_transform = players_[0]->GetParentTransform(); // TODO assumes player index 0
	
	auto server_buf = static_cast<char*>(malloc(GetBufSize(&sheader)));

	printf("server size %d", sheader.num_models);
	serverSerialize(server_buf, &sheader, model_infos.data());
	return std::make_pair(server_buf, GetBufSize(&sheader));
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

double GameManager::GetFixedDeltaTime() {
	const std::chrono::duration<double> elapsed_seconds = curr_time_ - last_time_;
    return elapsed_seconds.count();
}

void GameManager::UpdateFixedDeltaTime()
{
	last_time_ = curr_time_;
	curr_time_ = std::chrono::steady_clock::now();
}