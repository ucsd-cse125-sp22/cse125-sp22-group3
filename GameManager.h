#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Player.h"
#include "Vegetable.h"
#include "Plot.h"
#include "GameEntity.h"
#include "PhysicsEngine.h" 
#include "Seed.h"
#include "Growth.h"
#include <chrono>

class GameManager : GameEntity
{
public:
    GameManager();
    explicit GameManager(std::vector<Player*> players);

    // Call Updates on GameState
    void FixedUpdate() override;
    void AddEntities(std::vector<GameEntity*> entities);

    // Player Properties
    void SetPlayerInput(glm::vec2 move_input, int player_index);
    void SetPlayerUse(int player_index);
    void SetPlayerDrop(int player_index);

    // Information
    std::pair<char*, int> GetServerBuf();

    // Physics Engine
    PhysicsEngine physics;

    // Game entities in the world
    std::vector<GameEntity*> game_entities{};

    // FixedDeltaTime is the time between logic updates on the server
    // Typically 30ms, but you never know
    static void UpdateFixedDeltaTime();
    static double GetFixedDeltaTime();

private:
    std::vector<Player*> players_{}; //TODO what the heck
    std::vector<Vegetable*> vegetables_{};
    std::vector<Plot*> plots_{};
    std::vector<Seed*> seeds_{};
    
    static std::chrono::steady_clock::time_point last_time_;
    static std::chrono::steady_clock::time_point curr_time_;
};

#endif