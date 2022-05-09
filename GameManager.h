#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "World.h"
#include "Player.h"
#include "Vegetable.h"
#include "Plot.h"
#include "GameEntity.h"
#include "PhysicsEngine.h" 
#include "Seed.h"
#include "NPC.h"
#include "Particle.h"
#include <chrono>

class GameManager : GameEntity
{
public:
    GameManager();
    explicit GameManager(std::vector<Player*> players);

    // Call Updates on GameState
    void FixedUpdate() override;
    static void AddEntities(std::vector<GameEntity*> entities);
    static void RemoveEntities(std::vector<GameEntity*> entities);
    static void RemovePhysicsObjects(std::vector<PhysicsObject*> physics_objects);

    // Player Properties
    void SetPlayerInput(glm::vec2 move_input, int player_index);
    void SetPlayerUse(int player_index);
    void SetPlayerDrop(int player_index);
    void SetPlayerSprint(int player_index, bool sprinting);
    void SetPlayerDance(int player_index);
    void SetPlayerBuy(int player_index, VegetableType bought_vegetable);
    void SetPlayerSell(int player_index);
    void SetClosePlayerUI(int player_index);

    // Information
    std::vector<std::pair<char*, int>> GetServerBuf();

    // Physics Engine
    static PhysicsEngine physics;

    // Game entities in the world
    static std::vector<GameEntity*> game_entities;

    static std::vector<PhysicsObject*> physics_objects;

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