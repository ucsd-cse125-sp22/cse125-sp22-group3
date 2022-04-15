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
        static bool getMoved();
        static void setMoved();
        static bool checkIdle();

        static bool justMoved;

        static bool InputManager::moveL;
        static bool InputManager::moveR;
        static bool InputManager::moveF;
        static bool InputManager::moveB;

	private:
       

};

#endif