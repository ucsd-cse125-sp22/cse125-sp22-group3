#include "main.h"

#include "Client.h"
#include <chrono>

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
	glfwSetKeyCallback(window, Window::keyCallback);

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
	glClearColor(0.0, 0.0, 0.0, 0.0);
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
	Client* client = new Client("localhost", "8686");

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

	// Loop while GLFW window should stay open.
	int status = 1;
	auto begin_time = std::chrono::steady_clock::now();
	while (!glfwWindowShouldClose(window) || status > 0)
	{
		char* dummy_data = "Hello :)))) ****";
		status = client->syncWithServer(dummy_data, strlen(dummy_data) + 1, [window](const char* recv_buf, size_t recv_len)
			{
				printf("Callback echo: %.*s\n", (unsigned int)recv_len, recv_buf);
				// Main render display callback. Rendering of objects is done here. (Draw)
				Window::displayCallback(window);

				// Idle callback. Updating objects, etc. can be done here. (Update)
				Window::idleCallback();
			});
		auto end_time = std::chrono::steady_clock::now(); 
		long long elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
		printf("Milliseconds passed: %d\n", elapsed_time_ms);
		begin_time = end_time;
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