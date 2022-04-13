#include "GameManager.h"

#include <utility>

double GameManager::curr_time_ = 0;
double GameManager::last_time_ = 0;

GameManager::GameManager(std::vector<Player> players)
{
	// Initialize Players
	players_ = players;
	for (Player& player : players_) {
		game_entities.push_back(&player);
	}
	
	// Get all the game entities in the world
	//gameEntities
}
void GameManager::FixedUpdate()
{
	for (GameEntity* entity : game_entities) {
		entity->FixedUpdate();
	}
}
void GameManager::Draw(const glm::mat4 view, const glm::mat4 projection, const GLuint shader)
{
	// TODO don't do this. Figure out how to draw all Drawable GameEntities.
	for (Player& player : players_) {
		player.Draw(view, projection, shader);
	}
}
void GameManager::SetPlayerInput(const glm::vec2 move_input, const int player_index)
{
	players_[player_index].move_input = move_input;
}

glm::vec3 GameManager::GetPlayerPosition(const int player_index) const
{
	return players_[player_index].GetPosition();
}

double GameManager::GetFixedDeltaTime() { return curr_time_ - last_time_; }

void GameManager::UpdateFixedDeltaTime()
{
	last_time_ = curr_time_;
	curr_time_ = glfwGetTime();

}