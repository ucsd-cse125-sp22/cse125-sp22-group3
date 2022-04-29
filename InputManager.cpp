#include "InputManager.h"

InputCommands InputManager::lastCmd = NONE;
bool InputManager::justMoved = false;
glm::vec2 InputManager::lastMovement = glm::vec2(0, 0);

InputManager::InputManager() {}

InputManager::~InputManager() {}

InputCommands InputManager::getLastCommand() {
	return lastCmd;
}

glm::vec2 InputManager::getLastMovement() {
	return lastMovement;
}

bool InputManager::getMoved() {
	return justMoved;
}

void InputManager::resetMoved() {
	justMoved = false;
}

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	justMoved = true;
	
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
			case GLFW_KEY_W:
				lastMovement -= glm::vec2(0,1);
				lastCmd = STOP_FORWARD;
				break;
			case GLFW_KEY_S:
				lastMovement -= glm::vec2(0,-1);
				lastCmd = STOP_BACKWARD;
				break;
			case GLFW_KEY_A:
				lastMovement -= glm::vec2(-1,0);
				lastCmd = STOP_LEFT;
				break;
			case GLFW_KEY_D:
				lastMovement -= glm::vec2(1,0);
				lastCmd = STOP_RIGHT;
				break;
			default: break;
		}
	}

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GL_TRUE); // Close the window.
		}

		switch (key)
		{
			case GLFW_KEY_W:
				lastMovement += glm::vec2(0,1);
				lastCmd = MOVE_FORWARD;
				break;
			case GLFW_KEY_S:
				lastMovement += glm::vec2(0,-1);
				lastCmd = MOVE_BACKWARD;
				break;
			case GLFW_KEY_A:
				lastMovement += glm::vec2(-1,0);
				lastCmd = MOVE_LEFT;
				break;
			case GLFW_KEY_D:
				lastMovement += glm::vec2(1,0);
				lastCmd = MOVE_RIGHT;
				break;
			case GLFW_KEY_F:
				lastCmd = USE;
				//justMoved = false;
				break;
			case GLFW_KEY_G:
				lastCmd = DROP;
				//justMoved = false;
				break;
			default: break;
		}
	}
}
