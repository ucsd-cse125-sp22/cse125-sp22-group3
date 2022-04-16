#include "GameManager.h"

#include <utility>


double GameManager::curr_time_ = 0;
double GameManager::last_time_ = 0;
glm::vec2 GameManager::move_input(0, 0);
std::vector<Player*> GameManager::players_;

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



void GameManager::SetPlayerInput(InputCommands c, const int player_index)
{
	//players_[player_index]->move_input = move_input;
	players_[player_index]->move_input = calcMoveInput(c);
}

glm::vec3 GameManager::GetPlayerPosition(const int player_index) const
{
	return players_[player_index]->GetPosition();
}

double GameManager::GetFixedDeltaTime() { return curr_time_ - last_time_; }

glm::vec2 GameManager::calcMoveInput(InputCommands c)
{
	//glm::vec2 move_input(0, 0);
		switch (c)
		{
			
		case NONE:
			move_input = glm::vec2(0, 0);
			break;
			
			
		case STOP_FORWARD:
			move_input = glm::vec2(move_input.x, 0);
			break;
		case STOP_BACKWARD:
			move_input = glm::vec2(move_input.x, 0);
			break;
		case STOP_LEFT:
			move_input = glm::vec2(0, move_input.y);
			break;
		case STOP_RIGHT:
			move_input = glm::vec2(0, move_input.y);
			break;
			
		case MOVE_FORWARD:
			move_input += glm::vec2(0, 1);
			break;
		case MOVE_BACKWARD:
			move_input += glm::vec2(0, -1);
			break;
		case MOVE_LEFT:
			move_input += glm::vec2(-1, 0);
			break;
		case MOVE_RIGHT:
			move_input += glm::vec2(1, 0);
			break;
		default: break;
	}

		if (move_input.x > 1.0) {
			move_input.x = 1.0;
		}
		else if (move_input.x < -1.0) {
			move_input.x = -1.0;
		}

		if (move_input.y > 1.0) {
			move_input.y = 1.0;
		}
		else if (move_input.y < -1.0) {
			move_input.y = -1.0;
		}
	return move_input;
}

void GameManager::UpdateFixedDeltaTime()
{
	last_time_ = curr_time_;
	curr_time_ = glfwGetTime();

}