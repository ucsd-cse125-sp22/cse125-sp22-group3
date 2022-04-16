#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Player.h"
#include "GameEntity.h"
#include "PhysicsEngine.h" 
#include "util.h"
#include <chrono>

class GameManager : GameEntity, Drawable
{
public:
    explicit GameManager(std::vector<Player*> players);

    // Call Updates on GameState
    void FixedUpdate() override;
    void Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) override;

    // Player Properties
    static void SetPlayerInput(InputCommands c, int player_index);
    
    glm::vec3 GetPlayerPosition(int player_index) const;

    // Physics Engine
    PhysicsEngine physics_;

    // Game entities in the world
    std::vector<GameEntity*> game_entities{};

    // FixedDeltaTime is the time between logic updates on the server
    // Typically 30ms, but you never know
    static void UpdateFixedDeltaTime();
    static double GetFixedDeltaTime();
    static glm::vec2 calcMoveInput(InputCommands c);

private:
    static std::vector<Player*> players_;
    
    static double last_time_;
    static double curr_time_;
    static glm::vec2 move_input;
};

#endif