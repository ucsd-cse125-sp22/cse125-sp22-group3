#include "Window.h"

// Game Manager
GameManager* Window::game = nullptr;

// Window input
glm::vec2 Window::move_input{0,0};

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "GLFW Starter Project";

// Objects to Render
Model* Window::bumbus;
Player* Window::player;

// Camera Matrices 
// Projection matrix:
glm::mat4 Window::projection; 

constexpr float camera_angle = glm::radians(45.0f);
constexpr float camera_dist = 30;
// View Matrix:
glm::vec3 Window::eyePos(0, 0, 20);	// Camera position.
glm::vec3 Window::lookAtPoint(0, 0, 0);		// The point we are looking at.
glm::vec3 Window::upVector(0, 1, 0);		// The up direction of the camera.
glm::mat4 Window::view = glm::lookAt(Window::eyePos, Window::lookAtPoint, Window::upVector);

// Shader Program ID
GLuint Window::shaderProgram; 
GLuint Window::modelShaderProgram; 
GLuint Window::animationShaderProgram; 


bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	modelShaderProgram = LoadShaders("shaders/model.vert", "shaders/model.frag");
	animationShaderProgram = LoadShaders("shaders/animation.vert", "shaders/animation.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	// Check the shader program.
	if (!modelShaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	if (!animationShaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	// load models
	bumbus = new Model("models/bumbus/bumbus.fbx");

	Player player_1{bumbus};
	Player player_2{bumbus};
	
	game = new GameManager({player_1, player_2});
	
	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	// delete player;

	// Delete the shader program.
	glDeleteProgram(shaderProgram);
	glDeleteProgram(modelShaderProgram);
	glDeleteProgram(animationShaderProgram);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	Window::projection = glm::perspective(glm::radians(60.0), 
								double(width) / (double)height, 1.0, 1000.0);
}

void Window::idleCallback()
{
	// Perform any necessary updates here
	GameManager::UpdateFixedDeltaTime();
	game->FixedUpdate();
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	eyePos = game->GetPlayerPosition(0) + glm::vec3(0,camera_dist,camera_dist);	// TODO implement angle.
	lookAtPoint = game->GetPlayerPosition(0); // The point we are looking at.
	view = glm::lookAt(Window::eyePos, Window::lookAtPoint, Window::upVector);

	// Render the objects
	// currObj->draw(view, projection, shaderProgram);
	game->Draw(view, projection, animationShaderProgram);
	
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();

	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key release.
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
			case GLFW_KEY_W:
				move_input -= glm::vec2(0,1);
				break;
			case GLFW_KEY_S:
				move_input -= glm::vec2(0,-1);
				break;
			case GLFW_KEY_A:
				move_input -= glm::vec2(-1,0);
				break;
			case GLFW_KEY_D:
				move_input -= glm::vec2(1,0);
				break;
			default: break;
		}
	}

	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GL_TRUE); // Close the window.
		}

		switch (key)
		{
			case GLFW_KEY_W:
				move_input += glm::vec2(0,1);
				break;
			case GLFW_KEY_S:
				move_input += glm::vec2(0,-1);
				break;
			case GLFW_KEY_A:
				move_input += glm::vec2(-1,0);
				break;
			case GLFW_KEY_D:
				move_input += glm::vec2(1,0);
				break;
			default: break;
		}
	}

	game->SetPlayerInput(move_input, 0);
	game->SetPlayerInput(glm::vec2{0, 0.25}, 1); //TODO test NPC
}

void Window::cursorCallback(GLFWwindow* window, double xpos, double ypos) 
{
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)) {
		std::cout << "RIGHT_CLICK" << std::endl;
	}

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)) {
		std::cout << "LEFT_CLICK" << std::endl;
	}
}