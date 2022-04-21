#include "main.h"

#include "./Network/Client.h"
#include "./Network/NetworkPacket.h"
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

	//TODO remove test
	Model pogo{"models/pogo/pogo.fbx"};
	
	// Loop while GLFW window should stay open and server han't closed connection
	while (!glfwWindowShouldClose(window) && status > 0)
	{
		// outgoing packet initialization
		struct ClientPacket out_packet;

		// check if keycallback was called, if it was, update player (bandaid fix to make movement feel better)
		if (InputManager::getMoved()) { // TODO determine when to send packet and when to skip
			out_packet.justMoved = InputManager::getMoved();
			out_packet.movement = InputManager::getLastMovement();
		}
		InputManager::resetMoved();
		
		out_packet.lastCommand = InputManager::getLastCommand();

		status = client->syncWithServer(&out_packet, sizeof(out_packet), [&pogo, window](char* recv_buf, size_t recv_len)
			{
				ServerHeader sheader{};
				const auto model_arr = static_cast<ModelInfo*>(malloc(reinterpret_cast<ServerHeader*>(recv_buf)->num_models * sizeof(ModelInfo)));
				serverDeserialize(recv_buf, &sheader, model_arr);

				const glm::mat4 player_transform = model_arr[sheader.player_model_id].parent_transform;
				const glm::vec3 player_pos = glm::vec3(player_transform[3][0], player_transform[3][1], player_transform[3][2])/player_transform[3][3];
				
				const glm::vec3 eye_pos = player_pos + glm::vec3(0,20,20);	// TODO implement angle.
				const glm::vec3 look_at_point = player_pos; // The point we are looking at.
				const glm::mat4 view = glm::lookAt(eye_pos, look_at_point, Window::upVector);

				for (int i = 0; i < sheader.num_models; i++)
				{
					const ModelInfo model_info = model_arr[i];
					switch (model_info.model)
					{
					case CHAR_POGO:
						pogo.draw(view, Window::projection, model_info.parent_transform, Window::animationShaderProgram);
						break;
					case CHAR_BUMBUS: break;
					case CHAR_SWAINKY: break;
					case CHAR_GILMAN: break;
					case VEG_CARROT: break;
					case VEG_CORN: break;
					}
				}
				/*
				ServerPacket in_packet;
				in_packet.deserializeFrom(recv_buf);

				// check if keycallback was called, if it was, update player (bandaid fix to make movement feel better)
                if (in_packet.justMoved) { // TODO conditional possibly redundant (remove from packet)
                    Window::game->SetPlayerInput(InputManager::getLastMovement(), 0);
                }
                if (in_packet.lastCommand == InputCommands::USE) {
                    Window::game->SetPlayerUse(0);
                }
                else if (in_packet.lastCommand == InputCommands::DROP) {
                    Window::game->SetPlayerDrop(0);
                }

				// Main render display callback. Rendering of objects is done here. (Draw)
				Window::displayCallback(window);	

				// Idle callback. Updating objects, etc. can be done here. (Update)
				Window::logicCallback();*/
			});
        
		auto end_time = std::chrono::steady_clock::now();
		long long elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
		printf("Elapsed time between ticks: %lld ms\n\n", elapsed_time_ms);
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