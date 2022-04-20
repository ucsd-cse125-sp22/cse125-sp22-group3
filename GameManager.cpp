#include "GameManager.h"

#include <utility>


double GameManager::curr_time_ = 0;
double GameManager::last_time_ = 0;

GameManager::GameManager(std::vector<Player*> players, std::vector<Vegetable*> vegetables)
{
	// Initialize Players
	players_ = players;
	int i = 0;
	for (Player* player : players_) {
		// Set Player Positions
		player->SetPosition({i * 10,0,0});
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



void GameManager::SetPlayerInput(glm::vec2 move_input, const int player_index)
{
	players_[player_index]->move_input = move_input;
}

void GameManager::SetPlayerUse(const int player_index) {
	// Get the triggered entity within the interact region
	Player* player = players_[player_index];
	auto entity_ = player->GetTriggeringEntity();

	if (entity_ != nullptr) {
		players_[player_index]->SetHoldEntity(entity_);
		auto vegetable = dynamic_cast<Vegetable*>(entity_);

		if (vegetable->canInteract(player) && vegetable->isHoldable) {
			vegetable->interact(player);
			player->SetHoldEntity(vegetable); // So we can get the vegetable type later
		}
	}
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