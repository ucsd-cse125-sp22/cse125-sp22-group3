#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Player.h"
#include "GameEntity.h"

class GameManager
{
public:
    GameManager();
    ~GameManager();

    // Game entities in the world
    std::vector<GameEntity> game_entities;

    static void UpdateFixedDeltaTime();
    static double GetFixedDeltaTime();
private:

    static double last_fixed_delta_time_;
    static double fixed_delta_time_;
};

#endif