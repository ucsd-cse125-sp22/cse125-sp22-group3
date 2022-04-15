#include "InputManager.h"

InputCommands InputManager::lastCmd = NONE;
bool InputManager::justMoved = false;
bool InputManager::moveL = false;
bool InputManager::moveR = false;
bool InputManager::moveF = false;
bool InputManager::moveB = false;

InputManager::InputManager()
{

}

InputManager::~InputManager()
{
}
InputCommands InputManager::getLastCommand(){
	
	return lastCmd;
}

bool InputManager::getMoved()
{

	return justMoved;
}

void InputManager::setMoved() {

	justMoved = false;
}

bool InputManager::checkIdle() {

	return !(moveL || moveR || moveF || moveB);
}

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key release.
	justMoved = true;
	
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			moveF = false;
			lastCmd = STOP_FORWARD;
			break;
		case GLFW_KEY_S:
			moveB = false;
			lastCmd = STOP_BACKWARD;
			break;
		case GLFW_KEY_A:
			moveL = false;
			lastCmd = STOP_LEFT;
			break;
		case GLFW_KEY_D:
			moveR = false;
			lastCmd = STOP_RIGHT;
			break;
		default: break;
		}
	}
	
	// Check for a key press.
	if (action == GLFW_PRESS||action==GLFW_REPEAT)
	{
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GL_TRUE); // Close the window.
		}
		else if (key == GLFW_KEY_F) {

			std::cout << "interact";
			lastCmd = USE;
		}

		switch (key)
		{
		case GLFW_KEY_W:
			moveF = true;
			lastCmd = MOVE_FORWARD;
			break;
		case GLFW_KEY_S:
			moveB = true;
			lastCmd = MOVE_BACKWARD;
			break;
		case GLFW_KEY_A:
			moveL = true;
			lastCmd = MOVE_LEFT;
			break;
		case GLFW_KEY_D:
			moveR = true;
			lastCmd = MOVE_RIGHT;
			break;
		default: break;
		}
	}
}


