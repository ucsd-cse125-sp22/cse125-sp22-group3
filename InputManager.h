#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
#include "Window.h"

class InputManager {
	public:
        InputManager();
        ~InputManager();
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	private:

};

#endif