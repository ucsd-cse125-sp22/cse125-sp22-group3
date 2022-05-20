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
#include "Shovel.h"
#include "Hoe.h"
#include "Poison.h"
#include "VeggieNet.h"
#include "WateringCan.h"
#include "Fertilizer.h"
#include "Glue.h"
#include "GlueOnGround.h"
#include "Soju.h"
#include "Oat.h"
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

    // Player Properties
    void SetPlayerInput(glm::vec2 move_input, int player_index);
    void SetPlayerUse(int player_index);
    void SetPlayerDrop(int player_index);
    void SetPlayerSprint(int player_index, bool sprinting);
    void SetPlayerDance(int player_index);
    void SetPlayerBuy(int player_index, VegetableType bought_vegetable);
    void SetPlayerBuy(int player_index, ModelEnum tool);
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

    void GameManager::StartGameTime();
    double GameManager::GetRemainingSeconds();

private:
    std::vector<Player*> players_{};
    std::vector<Vegetable*> vegetables_{};
    std::vector<Plot*> plots_{};
    std::vector<Seed*> seeds_{};
    
    static std::chrono::steady_clock::time_point last_time_;
    static std::chrono::steady_clock::time_point curr_time_;
    std::chrono::steady_clock::time_point game_end_time_;
    bool isStarted = false;
    float timeToSpawnEggplant = 540; // default spawn 6 minutes in
    bool eggplantSpawned = false;

    // winning info
    float timeToEndGame = 0; // TODO: set to end of game, set to 800 for debugging purposes
    bool podiumSpawned = false;
    glm::vec3 goldPosition = glm::vec3(0, 10, -20);
    glm::vec3 silverPosition = glm::vec3(-5, 7, -20);
    glm::vec3 bronzePosition = glm::vec3(5, 4, -20);
    glm::vec3 loserPosition = glm::vec3(-10, 0, -15);
    glm::vec3 podiumPosition = glm::vec3(0, -1, -20);
};

#endif