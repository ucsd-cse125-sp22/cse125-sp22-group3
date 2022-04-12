#include "GameManager.h"

double GameManager::fixed_delta_time_ = 0;
double GameManager::last_fixed_delta_time_ = 0;

GameManager::GameManager() {
	// Get all the game entities in the world
	//gameEntities
}
double GameManager::GetFixedDeltaTime() { return fixed_delta_time_ - last_fixed_delta_time_; }

void GameManager::UpdateFixedDeltaTime()
{
	last_fixed_delta_time_ = fixed_delta_time_;
	fixed_delta_time_ = glfwGetTime();

}