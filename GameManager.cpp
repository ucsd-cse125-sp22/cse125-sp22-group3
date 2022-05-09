#include "GameManager.h"

#include "Network/NetworkPacket.h"

std::chrono::steady_clock::time_point GameManager::curr_time_ = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point GameManager::last_time_ = std::chrono::steady_clock::now();

std::vector<GameEntity*> GameManager::game_entities = {};
PhysicsEngine GameManager::physics = PhysicsEngine();
std::vector<PhysicsObject*> GameManager::physics_objects = {};


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
		//player->SetWorldPosition({i * 10,0,0});
		// Add Players to Entities list
		GameManager::game_entities.push_back(player);
		//i++;
	}

	// Instantiate Physics Engine
	//std::vector<PhysicsObject*> physics_objects;
	for (Player* player : players_) {
		physics_objects.push_back(player);
	}
	physics = PhysicsEngine(physics_objects);
}

void GameManager::FixedUpdate()
{
	GameManager::UpdateFixedDeltaTime();
	for (GameEntity* entity : GameManager::game_entities) {
		entity->FixedUpdate();
	}

	printf("size: %d\n", physics.moving_collidables_.size());

	// Check collisions
	GameManager::physics.Compute();
}

void GameManager::AddEntities(std::vector<GameEntity*> entities)
{
	for (GameEntity* entity : entities) {
		GameManager::game_entities.push_back(entity);
		if (auto phys_obj = dynamic_cast<PhysicsObject*>(entity)) {
			GameManager::physics.AddPhysObject(phys_obj);
		}
	}
}

void GameManager::RemoveEntities(std::vector<GameEntity*> entities) {
	for (GameEntity* entity : entities) {
		auto iter = std::find(GameManager::game_entities.begin(), GameManager::game_entities.end(), entity);
		GameManager::game_entities.erase(iter);
	}
}

void GameManager::RemovePhysicsObjects(std::vector<PhysicsObject*> phys_objects) {
	for (PhysicsObject* phys_obj : physics.moving_collidables_) {
		auto iter = std::find(physics.moving_collidables_.begin(), physics.moving_collidables_.end(), phys_obj);
		physics.moving_collidables_.erase(iter);
		//GameManager::physics.RemovePhysObject(iter);
	}
}

std::vector<std::pair<char*, int>> GameManager::GetServerBuf()
{
	std::vector<ModelInfo> model_infos;
	for (GameEntity* entity : GameManager::game_entities) {

		auto drawable = dynamic_cast<Drawable*>(entity);
		if (drawable) {
			model_infos.push_back(ModelInfo{
				reinterpret_cast<uintptr_t>(entity),
				drawable->GetModelEnum(),
				drawable->GetAniMode(),
				drawable->GetParentTransform(),
				dynamic_cast<Player*>(entity) != nullptr
			});
		}
	}

	std::vector<std::pair<char*, int>> out_vec;
	for (int client_idx = 0; client_idx < NUM_CLIENTS; client_idx++) {
		ServerHeader sheader{};
		sheader.num_models = model_infos.size();
		sheader.player_transform = players_[client_idx]->GetParentTransform();
		sheader.player_sprinting = players_[client_idx]->sprint;
		sheader.ui_open = players_[client_idx]->ui_open;
		for (int i = 0; i < NUM_CLIENTS; i++) {
			sheader.balance[i] = players_[i]->curr_balance;
		}

		auto server_buf = static_cast<char*>(malloc(GetBufSize(&sheader)));
		serverSerialize(server_buf, &sheader, model_infos.data());
		out_vec.push_back(std::make_pair(server_buf, static_cast<int>(GetBufSize(&sheader))));
	}
	return out_vec;
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
void GameManager::SetPlayerSprint(const int player_index, const bool sprinting)
{
	players_[player_index]->sprint = sprinting;
}
void GameManager::SetPlayerDance(const int player_index)
{
	players_[player_index]->Dance();
}

void GameManager::SetPlayerBuy(int player_index, VegetableType bought_vegetable)
{
	players_[player_index]->Buy(bought_vegetable);
}

void GameManager::SetPlayerSell(int player_index)
{
	players_[player_index]->Sell();
}

void GameManager::SetClosePlayerUI(int player_index) {
	players_[player_index]->CloseUI();
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