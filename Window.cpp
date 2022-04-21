#include "Window.h"

// Game Manager
GameManager* Window::game = nullptr;

// Window input
//glm::vec2 Window::move_input{0,0};

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "GLFW Starter Project";

// Objects to Render
Player* Window::player;
DepthMap* Window::dm;

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
GLuint Window::shadowShaderProgram; 


bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	modelShaderProgram = LoadShaders("shaders/model.vert", "shaders/model.frag");
	animationShaderProgram = LoadShaders("shaders/animation.vert", "shaders/animation.frag");
	shadowShaderProgram = LoadShaders("shaders/shadows.vert", "shaders/shadows.frag");

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

	if (!shadowShaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	dm = new DepthMap(-20.0f, 20.0f);
	// load models
	Model bumbus = Model("models/bumbus/bumbus.fbx");
	Model pogo = Model("models/pogo/pogo.fbx");
	Model swainky = Model("models/swainky/swainky.fbx");
	Model gilma = Model("models/gilma/char4.fbx");
	Model carrot = Model("models/carrot/carrot.fbx"); // PLACEHOLDER
	Model corn = Model("models/corn/corn.fbx"); // PLACEHOLDER
	Model tree = Model("models/tree/tree.fbx"); // PLACEHOLDER

	Player* player_1 = new Player(bumbus);
	Player* player_2 = new Player(pogo);
	Player* player_3 = new Player(swainky);
	Player* player_4 = new Player(gilma);
	//Player* player_3 = new Player(pogo_2);
	Vegetable* vegetable_1 = new Vegetable(VegetableType::CARROT, carrot);
	Vegetable* vegetable_2 = new Vegetable(VegetableType::CORN, corn);
	Vegetable* vegetable_3 = new Vegetable(VegetableType::CORN, tree);
	
	//game = new GameManager({player_1, player_2, player_3});
	//game = new GameManager({ player_1, player_2 }, {});
	// 
	// Changed GameManager to include a list of vegetables for testing, we can revert back
	game = new GameManager({ player_1, player_2, player_3, player_4 }, { vegetable_1, vegetable_2, vegetable_3 });
	
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
	glDeleteProgram(shadowShaderProgram);
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

void Window::logicCallback()
{
	// Perform any necessary updates here
	GameManager::UpdateFixedDeltaTime();
	game->FixedUpdate();
}

float Lerp(const float a, const float b, const float f) //TODO move to a more global scope
{
	return a + f * (b - a);
}

void Window::displayCallback(GLFWwindow* window)
{
	// render Depth map for shadows
	dm->draw();	
	game->Draw(shadowShaderProgram);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport and render actual scene
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	eyePos = game->GetPlayerPosition(0) + glm::vec3(0,camera_dist,camera_dist);	// TODO implement angle.
	lookAtPoint = game->GetPlayerPosition(0); // The point we are looking at.
	view = glm::lookAt(Window::eyePos, Window::lookAtPoint, Window::upVector);
	
	// Render the objects
	game->Draw(view, projection, animationShaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();

	// Swap buffers.
	glfwSwapBuffers(window);
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

void Window::renderDepthMap() {
	glUseProgram(modelShaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DepthMap::dm);
	glUniform1i(glGetUniformLocation(shadowShaderProgram, "shadowMap"), 0);
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glUseProgram(0);
}