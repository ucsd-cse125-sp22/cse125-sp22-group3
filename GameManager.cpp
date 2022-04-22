#include "GameManager.h"

#include "Network/NetworkPacket.h"

double GameManager::curr_time_ = 0;
double GameManager::last_time_ = 0;

GameManager::GameManager()
{
	physics_ = PhysicsEngine();
}
GameManager::GameManager(std::vector<Player*> players, std::vector<Vegetable*> vegetables)
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

	// Testing vegetables, eventually would want to include a vector of GameEntities instead of separating each?
	vegetables_ = vegetables;
	for (Vegetable* vegetable : vegetables_) {
		// Set Player Positions
		vegetable->SetPosition({ i * 15,5,0 });
		game_entities.push_back(vegetable);
		game_entities.back()->type = EntityType::VEGETABLE;
		i++;
	}

	// Instantiate Physics Engine
	std::vector<PhysicsObject*> physics_objects;
	for (Player* player : players_) {
		physics_objects.push_back(player);
	}
	for (Vegetable* vegetable : vegetables_) {
		physics_objects.push_back(vegetable);
	}
	physics_ = PhysicsEngine(physics_objects);

}

void GameManager::FixedUpdate()
{
	for (GameEntity* entity : game_entities) {
		entity->FixedUpdate();
	}

	// Check collisions
	physics_.Compute();
}
char* GameManager::GetServerBuf()
{
	std::vector<ModelInfo> model_infos;
	model_infos.push_back(ModelInfo{ 0, CHAR_POGO, players_[0]->GetParentTransform() });

	ServerHeader sheader{};
	sheader.num_models = model_infos.size();
	sheader.player_model_id = 0;
	
	auto server_buf = static_cast<char*>(malloc(GetBufSize(&sheader)));
	serverSerialize(server_buf, &sheader, model_infos.data());
	return server_buf;
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

void GameManager::SetPlayerInput(glm::vec2 move_input, const int player_index)
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

double GameManager::GetFixedDeltaTime() { return curr_time_ - last_time_; }


void GameManager::UpdateFixedDeltaTime()
{
	last_time_ = curr_time_;
	curr_time_ = glfwGetTime();

}