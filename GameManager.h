#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Player.h"
#include "Vegetable.h"
#include "Plot.h"
#include "GameEntity.h"
#include "PhysicsEngine.h" 
#include "Seed.h"
#include <chrono>

class GameManager : GameEntity, public Drawable
{
public:
    GameManager();
    explicit GameManager(std::vector<Player*> players, std::vector<Vegetable*> vegetables, std::vector<Plot*> plots);
    explicit GameManager(std::vector<Player*> players, std::vector<Vegetable*> vegetables, std::vector<Plot*> plots, std::vector<Seed*> seeds);

    // Call Updates on GameState
    void FixedUpdate() override;
    std::pair<char*, int> GetServerBuf();
    void Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) override;
    void Draw(GLuint shader) override;

    // Player Properties
    void SetPlayerInput(glm::vec2 move_input, int player_index);
    void SetPlayerUse(int player_index);
    void SetPlayerDrop(int player_index);
    
    glm::vec3 GetPlayerPosition(int player_index) const;

    // Physics Engine
    PhysicsEngine physics_;

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