#include "InputManager.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key release.
	if (action == GLFW_RELEASE)
	{
		// move player forward/backward
		if (key == GLFW_KEY_W) {
			//player->StopMovingForward();
			std::cout<<"stop moving forward\n";
		}

		else if (key == GLFW_KEY_S) {
			//player->StopMovingBackward();
			std::cout<<"stop moving backward\n";
		}

		if (key == GLFW_KEY_A) {
			//player->StopTurningLeft();
			std::cout<<"stop moving left\n";
		}

		else if (key == GLFW_KEY_D) {
			//player->StopTurningRight();
            std::cout<<"stop moving right\n";
		}
	}

	// Check for a key press.
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GL_TRUE); // Close the window.
		}

		else {
			// move player forward/backward/left/right
			if (key == GLFW_KEY_W) {
				//player->Forward();
                std::cout<<"moving forward\n";
			}

			else if (key == GLFW_KEY_S) {
				//player->Backward();
                std::cout<<"moving backward\n";
			}

			if (key == GLFW_KEY_A) {
				//player->Left();
                std::cout<<"moving left\n";
			}

			else if (key == GLFW_KEY_D) {
				//player->Right();
                std::cout<<"moving right\n";
			}
		}
	}
}
