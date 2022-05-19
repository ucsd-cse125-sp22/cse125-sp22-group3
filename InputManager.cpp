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
	justMoved = true;
	lastCmd = NONE;
}

void InputManager::CursorCallback(GLFWwindow* window,const double xpos,const double ypos) 
{
	current_cursor = glm::vec2{xpos, ypos};
	
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)) {
		std::cout << "RIGHT_CLICK" << std::endl;
	}

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)) {
		std::cout << "LEFT_CLICK" << std::endl;
	}
}
glm::vec2 InputManager::GetCursorDelta()
{
	const glm::vec2 delta = current_cursor - last_cursor;
	last_cursor = current_cursor;
	return delta;
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
			case GLFW_KEY_LEFT_SHIFT:
				lastCmd = STOP_SPRINT;
				break;
			default: break;
		}
	}

	if (action == GLFW_PRESS)
	{
		// Yeeted by Danica
		/**
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GL_TRUE); // Close the window.
		}
		*/

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
			case GLFW_KEY_LEFT_SHIFT:
				lastCmd = SPRINT;
				break;
			case GLFW_KEY_X:
				lastCmd = DANCE_CMD;
				break;
			case GLFW_KEY_ESCAPE: 
				lastCmd = CLOSE_UI;
				break;
			default: break;
		}
	}
}