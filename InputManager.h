#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
#include "Window.h"
#include "util.h"

class InputManager {
	public:
        static InputCommands lastCmd;
        InputManager();
        ~InputManager();
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static InputCommands getLastCommand();
        static glm::vec2 getLastMovement();

        static bool getMoved();
        static void resetMoved();
        static void CursorCallback(GLFWwindow* window, double xpos, double ypos);
		static glm::vec2 GetCursorDelta();

        static bool justMoved;
        static glm::vec2 lastMovement;
		inline static glm::vec2 last_cursor = glm::vec2{0.f,0.f};
		inline static glm::vec2 current_cursor = glm::vec2{0.f,0.f};

		inline static constexpr float camera_speed = 0.1f;
		inline static constexpr float camera_dist = 42.f;
	private:
       

};

#endif