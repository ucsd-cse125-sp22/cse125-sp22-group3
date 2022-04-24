#include "main.h"
#include "UI/GUI.h"
#include "./Network/Client.h"
#include "./Network/NetworkPacket.h"
#include "./Network/ServerMain.cpp"
#include "Model.h"

#include <chrono>
#include <map>

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

int main(int argc, char* argv[])
{
	if (argc > 1 && (std::string(argv[1]) == "server-build")) { //TODO Add Seperate Project
		ServerMain();
		return 0;
	}
	
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

	// initialize IMGUI 
	GUI::initializeGUI(window);
	GUI::initializeImage();

	// Initialize network client interface
	Client* client = new Client(SERVER_ADDRESS, SERVER_PORT);

	auto begin_time = std::chrono::steady_clock::now();
	int status = 1;

	//TODO remove test
	// load models
	Model bumbus = Model(CHAR_BUMBUS);
	Model pogo = Model(CHAR_POGO);
	Model swainky = Model(CHAR_SWAINKY);
	Model gilma = Model(CHAR_GILMAN);
	Model carrot = Model(VEG_CARROT); // PLACEHOLDER
	
	std::map<uintptr_t, Model*> model_map; // TODO change into smart pointer

	// Loop while GLFW window should stay open and server han't closed connection
	while (!glfwWindowShouldClose(window) && status > 0)
	{
		// outgoing packet initialization
		ClientPacket out_packet;

		// check if keycallback was called, if it was, update player (bandaid fix to make movement feel better)
		if (InputManager::getMoved()) { // TODO determine when to send packet and when to skip
			out_packet.justMoved = InputManager::getMoved();
			out_packet.movement = InputManager::getLastMovement();
			out_packet.lastCommand = InputManager::getLastCommand();
		}
		InputManager::resetMoved();
		out_packet.player_index = 0;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		status = client->syncWithServer(&out_packet, sizeof(out_packet), [&](char* recv_buf, size_t recv_len)
			{
				ServerHeader* sheader;
				ModelInfo* model_arr;
				serverDeserialize(recv_buf, &sheader, &model_arr);

				//Rendering Code
				const glm::mat4 player_transform = sheader->player_transform;
				const glm::vec3 player_pos = glm::vec3(player_transform[3][0], player_transform[3][1], player_transform[3][2])/player_transform[3][3];
				
				const glm::vec3 eye_pos = player_pos + glm::vec3(0,30,30);	// TODO implement angle.
				const glm::vec3 look_at_point = player_pos; // The point we are looking at.
				const glm::mat4 view = glm::lookAt(eye_pos, look_at_point, Window::upVector);

				for (int i = 0; i < sheader->num_models; i++)
				{
					const ModelInfo model_info = model_arr[i];

					if (model_map.count(model_info.model_id) == 0) {
						model_map[model_info.model_id] = new Model(model_info.model);
					}

					model_map[model_info.model_id]->setAnimationMode(model_info.modelAnim);
					model_map[model_info.model_id]->draw(view, Window::projection, model_info.parent_transform, Window::animationShaderProgram);
				}

				free(sheader);
				free(model_arr);
			});
		
		// Gets events, including input such as keyboard and mouse or window resizing
		glfwPollEvents();

		//IMGUI rendering
		Window::show_GUI = GUI::renderUI(Window::show_GUI);

		// Swap buffers.
		glfwSwapBuffers(window);
        
		/*
		char* dummy_data = "Hello from the Networking Team";
		status = client->syncWithServer(dummy_data, strlen(dummy_data) + 1, [window](const char* recv_buf, size_t recv_len)
		{
			printf("Callback echo: %.*s\n", (unsigned int)recv_len, recv_buf);

			// check if keycallback was called, if it was, update player (bandaid fix to make movement feel better)

			if (InputManager::getMoved) {
				InputCommands inCom = InputManager::getLastCommand();
				if (InputManager::checkIdle())
					inCom = NONE;
				GameManager::SetPlayerInput(inCom, 0);
			}

			InputManager::setMoved();
			//GameManager::SetPlayerInput(inCom, 0);

			//IMGUI rendering
			ImGui_ImplGlfwGL3_NewFrame();
			
			ImGui::Begin("My Name is Window, IMGUI Window");
			ImGui::Text("Hello there adeventurer!");
			ImGui::End();

			// Main render display callback. Rendering of objects is done here. (Draw)
			Window::displayCallback(window);

			// Idle callback. Updating objects, etc. can be done here. (Update)
			

			Window::idleCallback();
		});
		*/

		auto end_time = std::chrono::steady_clock::now();
		long long elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
		printf("Elapsed time between ticks: %lld ms\n\n", elapsed_time_ms);
		begin_time = end_time;
	}

	// destroy objects created
	Window::cleanUp();
	for (auto iter = model_map.begin(); iter != model_map.end(); iter++) {
		delete iter->second;
	}

	// Cleanup ImGui
	GUI::cleanUp();

	// Destroy the window.
	glfwDestroyWindow(window);

	// Terminate GLFW.
	glfwTerminate();

	delete client;

	exit(EXIT_SUCCESS);
}