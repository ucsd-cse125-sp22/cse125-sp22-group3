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
        static bool checkIdle();

        static bool moveF;
        static bool moveB;
        static bool moveL;
        static bool moveR;

	private:
       

};

#endif