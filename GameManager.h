#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Player.h"
#include "GameEntity.h"
#include "../../../../../Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/VC/Tools/MSVC/14.29.30133/include/chrono"

class GameManager
{
public:
    GameManager();
    ~GameManager();

    // Game entities in the world
    std::vector<GameEntity> game_entities;

    // FixedDeltaTime is the time between logic updates on the server
    // Typically 30ms, but you never know
    static void UpdateFixedDeltaTime();
    static double GetFixedDeltaTime();
private:
    static double last_time_;
    static double curr_time_;
};

#endif