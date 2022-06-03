#include "Window.h"
#include "Model.h"

#include "UI/GUI.h"
#include "main.h"

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
FBO* Window::postprocessing;
FBO* Window::bloom;

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
GLuint Window::worldShaderProgram; 
GLuint Window::waterShaderProgram; 
GLuint Window::leafShaderProgram; 
GLuint Window::modelShaderProgram; 
GLuint Window::animationShaderProgram; 
GLuint Window::debugShaderProgram; 
GLuint Window::domeShaderProgram; 
GLuint Window::shadowShaderProgram; 
GLuint Window::blurShaderProgram;
GLuint Window::finalShaderProgram;
GLuint Window::particleShaderProgram;

// Shader to Program
std::map<ModelEnum, GLuint> Window::modelShader;

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	worldShaderProgram = LoadShaders("shaders/world.vert", "shaders/world.frag");
	waterShaderProgram = LoadShaders("shaders/water.vert", "shaders/water.frag");
	leafShaderProgram = LoadShaders("shaders/leaves.vert", "shaders/world.frag");
	modelShaderProgram = LoadShaders("shaders/model.vert", "shaders/model.frag");
	animationShaderProgram = LoadShaders("shaders/animation.vert", "shaders/animation.frag");
	shadowShaderProgram = LoadShaders("shaders/shadows.vert", "shaders/shadows.frag", "shaders/shadows.geom");
	blurShaderProgram = LoadShaders("shaders/blur.vert", "shaders/blur.frag");
	finalShaderProgram = LoadShaders("shaders/final.vert", "shaders/final.frag");
	particleShaderProgram = LoadShaders("shaders/particles.vert", "shaders/particles.frag");
	debugShaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	domeShaderProgram = LoadShaders("shaders/dome.vert", "shaders/dome.frag");

	// Check the shader program.
	if (!worldShaderProgram)
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

	if (!blurShaderProgram) {
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	if (!finalShaderProgram) {
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	if (!particleShaderProgram) {
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	if (!leafShaderProgram) {
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	// Use later, in case many objects need many shaders
	modelShader = {
	{ CHAR_BUMBUS, animationShaderProgram },
	{ CHAR_POGO, animationShaderProgram },
	{ CHAR_SWAINKY, animationShaderProgram },
	{ CHAR_GILMAN, animationShaderProgram }, // TODO rename
	{ CHAR_NPC, animationShaderProgram },

	{ VEG_CABBAGE, animationShaderProgram  },
	{ VEG_CARROT, animationShaderProgram  },
	{ VEG_CORN, animationShaderProgram  },
	{ VEG_RADISH, animationShaderProgram  },
	{ VEG_TOMATO, animationShaderProgram  },
	{ VEG_GOLDEN_EGGPLANT, animationShaderProgram  },

	{ WATERING_CAN, animationShaderProgram  },
	{ POISON, animationShaderProgram  },
	{ SHOVEL, animationShaderProgram  },
	{ HOE, animationShaderProgram  },
	{ NET, animationShaderProgram},
	{ GLUE, animationShaderProgram},
	{ FERTILIZER, animationShaderProgram},
	{ GLUE_ON_GROUND,animationShaderProgram},
	{ OATS, animationShaderProgram  },
	{ SOJU, animationShaderProgram  },

	{ WORLD_PLOT_RED, animationShaderProgram  },
	{ WORLD_PLOT_BLUE, animationShaderProgram },
	{ WORLD_PLOT_GREEN, animationShaderProgram  },
	{ WORLD_PLOT_YELLOW, animationShaderProgram  },

	{ WORLD_SEED_CABBAGE, animationShaderProgram  },
	{ WORLD_SEED_CARROT, animationShaderProgram  },
	{ WORLD_SEED_CORN, animationShaderProgram  },
	{ WORLD_SEED_RADISH, animationShaderProgram  },
	{ WORLD_SEED_TOMATO, animationShaderProgram  },

	{ WORLD_FLAG_CABBAGE, animationShaderProgram  },
	{ WORLD_FLAG_CARROT, animationShaderProgram },
	{ WORLD_FLAG_CORN, animationShaderProgram  },
	{ WORLD_FLAG_RADISH, animationShaderProgram  },
	{ WORLD_FLAG_TOMATO, animationShaderProgram  },
	{ WORLD_FLAG_POISON, animationShaderProgram  },

	{INDICATOR_WATER,animationShaderProgram},
	{INDICATOR_FERTILIZER,animationShaderProgram},

	{ DEBUG_CIRCLE, debugShaderProgram },
	{ DEBUG_SQUARE, debugShaderProgram },

	{ WORLD_MAP, worldShaderProgram },
	{ WORLD_DOME, domeShaderProgram },
	{ WORLD_WATER, waterShaderProgram },
	{ WORLD_LEAVES, leafShaderProgram },
	{ WORLD_GRASS, leafShaderProgram },
	{ WORLD_PODIUM, worldShaderProgram },

	{ PARTICLE_GLOW, particleShaderProgram },
	{ PARTICLE_DUST, particleShaderProgram },
	{ PARTICLE_FIREFLIES, particleShaderProgram },
	{ PARTICLE_FIREFLIES_PURPLE, particleShaderProgram },
	{ PARTICLE_FIREFLIES_ORANGE, particleShaderProgram },
	{ PARTICLE_FIREFLIES_BLUE, particleShaderProgram },
	{ PARTICLE_FIREFLIES_RED, particleShaderProgram },
	{ PARTICLE_FIREFLIES_PINK, particleShaderProgram },
	{ PARTICLE_FIREFLIES_GREEN, particleShaderProgram },
	{ PARTICLE_FIREFLIES_WHITE, particleShaderProgram },

	{BALLOON_YELLOW, animationShaderProgram  },
	{BALLOON_RED, animationShaderProgram  },
	{BALLOON_BLUE, animationShaderProgram  },
	{BALLOON_PURPLE, animationShaderProgram  },
	{BALLOON_PINK, animationShaderProgram  },
	{BALLOON_GREEN, animationShaderProgram  },
	};
			
	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	// delete player;

	// Delete the shader program.
	glDeleteProgram(worldShaderProgram);
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
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);

	//TODO: change it back to fullscreen
	//GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, monitor, NULL);
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, monitor, NULL);

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
	if (height != 0) {
		Window::projection = glm::perspective(glm::radians(60.0),
			double(width) / (double)height, 1.0, 1000.0);
	}
	GUI::updateDisplayRatio(width, height); 
	if (Window::bloom != nullptr) {
		Window::bloom->resize(width, height);
	}
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

glm::vec3 Window::GetEyePos()
{
	return eyePos;
}

void Window::renderDepthMap() {
	// glUseProgram(modelShaderProgram);

	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D_ARRAY, FBO::dm);
	// glUniform1i(glGetUniformLocation(modelShaderProgram, "map"), 0);
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
	// glUseProgram(0);
}