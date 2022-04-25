#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Player.h"
#include "Vegetable.h"
#include "Plot.h"
#include "main.h"
#include "shader.h"
#include "GameManager.h"
#include "DepthMap.h"

class Window
{
public:

	// Window Properties
	static int width;
	static int height;
	static const char* windowTitle;

	// Game Manager
	static GameManager* game; //TODO move all of this somewhere else
	
	// Window Input
	//static glm::vec2 move_input; //TODO move this somewhere else
	
	// Objects to Render
	static Player* player;

	// Camera Matrices
	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::vec3 eyePos, lookAtPoint, upVector;

	static DepthMap* dm;

	// Shader Program ID
	static GLuint shaderProgram;
	static GLuint modelShaderProgram;
	static GLuint shadowShaderProgram;
	static GLuint animationShaderProgram;


	// Constructors and Destructors
	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();

	// Window functions
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);

	// Draw and Update functions
	static void logicCallback();
	static void displayCallback(GLFWwindow*);

	static void renderDepthMap();

	// Callbacks
	/*
	 * Where we take user input from the keyboard
	 */
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	/*
	 * Where we take user input from the mouse
	 */
	static void cursorCallback(GLFWwindow* window, double xpos, double ypos);

};

#endif
