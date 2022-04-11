#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Player.h"
#include "GameEntity.h"

class GameManager {
	public:
		GameManager();
		~GameManager();

		// Game entities in the world
		GameEntity gameEntities[];
	private:
};

#endif