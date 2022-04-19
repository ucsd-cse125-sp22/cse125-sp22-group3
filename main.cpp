#include "main.h"

#include "./Network/Client.h"
#include <chrono>


#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT "8686"

void error_callback(int error, const char* description)
{
	// Print error.
	std::cerr << description << std::endl;
}

void setup_callbacks(GLFWwindow* window)
{
	// Set the error callback.
	glfwSetErrorCallback(error_callback);

	// Set the window resize callback.
	glfwSetWindowSizeCallback(window, Window::resizeCallback);

	// Set the key callback.
	//glfwSetKeyCallback(window, Window::keyCallback);
	glfwSetKeyCallback(window, InputManager::keyCallback);

	// Set cursor callback
	glfwSetCursorPosCallback(window, Window::cursorCallback);
}

void setup_opengl_settings()
{
	// Enable depth buffering.
	glEnable(GL_DEPTH_TEST);

	// Related to shaders and z value comparisons for the depth buffer.
	glDepthFunc(GL_LEQUAL);

	// Set polygon drawing mode to fill front and back of each polygon.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Set clear color to black.
	glClearColor(212.f/255, 242.f/255, 148.f/255, 1.0);
}

void print_versions()
{
	// Get info of GPU and supported OpenGL version.
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL version supported: " << glGetString(GL_VERSION)
		<< std::endl;

	//If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
	std::cout << "Supported GLSL version is: " <<
		glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

int main(void)
{
	// Create the GLFW window.
	GLFWwindow* window = Window::createWindow(640, 480);
	if (!window)
		exit(EXIT_FAILURE);

	// Print OpenGL and GLSL versions.
	print_versions();

	// Setup callbacks.
	setup_callbacks(window);

	// Setup OpenGL settings.
	setup_opengl_settings();

	// Initialize the shader program; exit if initialization fails.
	if (!Window::initializeProgram())
		exit(EXIT_FAILURE);

	// Initialize objects/pointers for rendering; exit if initialization fails.
	if (!Window::initializeObjects())
		exit(EXIT_FAILURE);

	// Initialize network client interface
	Client* client = new Client(SERVER_ADDRESS, SERVER_PORT);

	auto begin_time = std::chrono::steady_clock::now();
	int status = 1;
	

	// Loop while GLFW window should stay open and server han't closed connection
	while (!glfwWindowShouldClose(window) && status > 0)
	{
		char* dummy_data = "Hello from the Networking Team";
		status = client->syncWithServer(dummy_data, strlen(dummy_data) + 1, [window](const char* recv_buf, size_t recv_len)
			{
				//printf("Callback echo: %.*s\n", (unsigned int)recv_len, recv_buf);

				// Get input from client and send it to the game manager
				Window::calculateInput();
			
				// Main render display callback. Rendering of objects is done here. (Draw)
				Window::displayCallback(window);	

				// Idle callback. Updating objects, etc. can be done here. (Update)
				Window::logicCallback();
			});
		auto end_time = std::chrono::steady_clock::now();
		long long elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
		//printf("Elapsed time between ticks: %d ms\n\n", elapsed_time_ms);
		begin_time = end_time;

		// uncomment for testing
		// Main render display callback. Rendering of objects is done here. (Draw)
		/*
		Window::displayCallback(window);

		// Idle callback. Updating objects, etc. can be done here. (Update)
		Window::idleCallback();
		*/
	}

	// destroy objects created
	Window::cleanUp();

	// Destroy the window.
	glfwDestroyWindow(window);

	// Terminate GLFW.
	glfwTerminate();

	delete client;

	exit(EXIT_SUCCESS);
}