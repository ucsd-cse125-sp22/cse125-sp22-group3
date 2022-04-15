#include "GameManager.h"

#include <utility>


double GameManager::curr_time_ = 0;
double GameManager::last_time_ = 0;
glm::vec2 GameManager::move_input(0, 0);
std::vector<Player*> GameManager::players_;

GameManager::GameManager(std::vector<Player*> players)
{
	// Initialize Players
	players_ = players;
	int i = 0;
	for (Player* player : players_) {
		// Set Player Positions
		player->SetPosition({i * 10,0,0});
		// Add Players to Entities list
		game_entities.push_back(player);
		i++;
	}
	
	// Get all the game entities in the world
	//gameEntities

	// Instantiate Physics Engine
	std::vector<PhysicsObject*> physics_objects;
	for (Player* player : players_) {
		physics_objects.push_back(player);
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
	// TODO don't do this. Figure out how to draw all Drawable GameEntities.
	for (Player* player : players_) {
		player->Draw(view, projection, shader);
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
	glm::vec2 move_input(0, 0);
		switch (c)
		{
			
		case NONE:
			move_input = glm::vec2(0, 0);
			break;
			
		case STOP_FORWARD:
			move_input -= glm::vec2(0, 1);
			break;
		case STOP_BACKWARD:
			move_input -= glm::vec2(0, -1);
			break;
		case STOP_LEFT:
			move_input -= glm::vec2(-1, 0);
			break;
		case STOP_RIGHT:
			move_input -= glm::vec2(1, 0);
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

	
	return move_input;
}

void GameManager::UpdateFixedDeltaTime()
{
	last_time_ = curr_time_;
	curr_time_ = glfwGetTime();

}