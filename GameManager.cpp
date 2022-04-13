#include "GameManager.h"

double GameManager::curr_time_ = 0;
double GameManager::last_time_ = 0;

GameManager::GameManager() {
	// Get all the game entities in the world
	//gameEntities
}

double GameManager::GetFixedDeltaTime() { return curr_time_ - last_time_; }

void GameManager::UpdateFixedDeltaTime()
{
	last_time_ = curr_time_;
	curr_time_ = glfwGetTime();

}