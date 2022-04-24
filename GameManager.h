#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Player.h"
#include "Vegetable.h"
#include "Plot.h"
#include "GameEntity.h"
#include "PhysicsEngine.h" 
#include "util.h"
#include <chrono>

class GameManager : GameEntity, Drawable
{
public:
    explicit GameManager(std::vector<Player*> players, std::vector<Vegetable*> vegetables, std::vector<Plot*> plots);

    // Call Updates on GameState
    void FixedUpdate() override;
    void Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) override;

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
    
    static double last_time_;
    static double curr_time_;
};

#endif