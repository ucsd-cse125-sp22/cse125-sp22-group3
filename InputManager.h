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
        static void setMoved();

        static bool justMoved;
        static glm::vec2 lastMovement;


	private:
       

};

#endif