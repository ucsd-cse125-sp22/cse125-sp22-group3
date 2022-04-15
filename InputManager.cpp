#include "InputManager.h"

InputCommands InputManager::lastCmd = NONE;
bool InputManager::moveF = false;
bool InputManager::moveR = false;
bool InputManager::moveL = false;
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

bool InputManager::checkIdle()
{

	if (moveF || moveB || moveL || moveR)
		return false;
	return true;
}


void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key release.
	
	if (action == GLFW_RELEASE)
	{
		// move player forward/backward
		if (key == GLFW_KEY_W) {
			//player->StopMovingForward();
			
			std::cout<<"stop moving forward\n";
			lastCmd = STOP_FORWARD;
			moveF = false;
		}

		else if (key == GLFW_KEY_S) {
			//player->StopMovingBackward();
			std::cout<<"stop moving backward\n";
			lastCmd = STOP_BACKWARD;
			moveB = false;
		}

		if (key == GLFW_KEY_A) {
			//player->StopTurningLeft();
			std::cout<<"stop moving left\n";
			lastCmd = STOP_LEFT;
			moveL = false;
		}

		else if (key == GLFW_KEY_D) {
			//player->StopTurningRight();
            std::cout<<"stop moving right\n";
			lastCmd = STOP_RIGHT;
			moveR = false;
		}
	}

	// Check for a key press.
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GL_TRUE); // Close the window.
		}
		else if (key == GLFW_KEY_F) {

			std::cout << "interact";
			lastCmd = USE;
		}

		else {
			// move player forward/backward/left/right
			if (key == GLFW_KEY_W) {
				//player->Forward();
				
                std::cout<<"moving forward\n";
				lastCmd = MOVE_FORWARD;
				moveF = true;
			}

			else if (key == GLFW_KEY_S) {
				//player->Backward();

                std::cout<<"moving backward\n";
				lastCmd = MOVE_BACKWARD;
				moveB = true;
			}

			if (key == GLFW_KEY_A) {
				//player->Left();
                std::cout<<"moving left\n";
				lastCmd = MOVE_LEFT;
				moveL = true;
			}

			else if (key == GLFW_KEY_D) {
				//player->Right();
                std::cout<<"moving right\n";
				lastCmd = MOVE_RIGHT;
				moveR = true;
			}
		}
	}
}


