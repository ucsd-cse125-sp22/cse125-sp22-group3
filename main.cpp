#include "main.h"
#include "UI/GUI.h"
#include "./Network/Client.h"
#include "./Network/NetworkPacket.h"
#include "./Network/ServerMain.cpp"
#include "ConfigReader.h"
#include <filesystem>
#include "Model.h"
#include <thread>         
#include <chrono>
#include <map>

namespace fs = std::filesystem;

bool GUI::show_loading; 

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

	// Enable transparency for meshes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

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

void preload_texture_files() {
	std::cout << "Start pre-loading... " << std::endl; 
	const char* load_dir = "models";
	int i = 0;
	for (auto& entry : fs::recursive_directory_iterator(load_dir)) {
		std::cout << entry.path() << std::endl;
		const char* epath = entry.path().string().c_str();
		int width, height, nrComponents; 
		unsigned char* data = stbi_load(epath, &width, &height, &nrComponents, 0);
		stbi_image_free(data);
		i++;
	}
	GUI::show_loading = false; 
}

void load_models(GLFWwindow* window) 
{
	Model tmp; 
	int size = PARTICLE_GLOW - CHAR_BUMBUS+2; 
	float progress = 1; 
	bool flip_image = true; // variable use to flip the image 
	for (ModelEnum i = CHAR_BUMBUS; i <= PARTICLE_GLOW; i = ModelEnum(i + 1)) {
		flip_image = GUI::renderProgressBar(progress / size, window, flip_image);
		tmp = Model(i); 
		progress++;
	}
	//// Model constructor internally has logic to cache meshes
	//Model BUMBUS = Model(CHAR_BUMBUS);
	////Model BUMBUS2 = Model(CHAR_BUMBUS);
	//Model POGO = Model(CHAR_POGO);
	//Model SWAINKY = Model(CHAR_SWAINKY);
	//Model GILMAN = Model(CHAR_GILMAN);

	//Model CABBAGE = Model(VEG_CABBAGE);
	//Model carrot = Model(VEG_CARROT);
	//Model corn = Model(VEG_CORN);
	//Model radish = Model(VEG_RADISH);
	//Model tomato = Model(VEG_TOMATO);

	//Model wpr = Model(WORLD_PLOT_RED);
	//Model wpb = Model(WORLD_PLOT_BLUE);
	//Model wpg = Model(WORLD_PLOT_GREEN);
	//Model wpy = Model(WORLD_PLOT_YELLOW);

	//Model wseed_carrot = Model(WORLD_SEED_CARROT);
	//Model wseed_corn = Model(WORLD_SEED_CORN);
	//Model wseed_cabbage = Model(WORLD_SEED_CABBAGE);
	//Model wseed_raddish = Model(WORLD_SEED_RADISH);
	//Model wseed_tomato = Model(WORLD_SEED_TOMATO);

	//Model wflag_carrot = Model(WORLD_FLAG_CARROT);
	//Model wflag_corn = Model(WORLD_FLAG_CORN);
	//Model wflag_cabbage = Model(WORLD_FLAG_CABBAGE);
	//Model wflag_raddish = Model(WORLD_FLAG_RADISH);
	//Model wflag_tomato = Model(WORLD_FLAG_TOMATO);

	//Model world_map = Model(WORLD_MAP);

	//this_thread::sleep_for(chrono::milliseconds(5000));
	//GUI::show_loading = false; 

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

	GUI::show_loading = true;
	//std::thread loadingThread(load_models, window);
	// initialize IMGUI 
	GUI::initializeGUI(window);
	GUI::initializeImage();

	// Initialize network client interface
	static std::unordered_map<std::string, std::string> client_config = ConfigReader::readConfigFile("server.cfg");
	Client* client = new Client(client_config["server_address"].c_str(), DEFAULT_PORT);

	//auto begin_time = std::chrono::steady_clock::now();
	int status = 1;

	std::map<uintptr_t, Model*> model_map; // TODO change into smart pointer
	//std::thread preload_thread(preload_texture_files); 
	//GUI::renderLoadScene(window);
	//preload_thread.join(); 
	//loadingThread.join(); 
	load_models(window); 
	Window::show_GUI = false; 

	int num_clients = 4;
	client->syncGameReadyToStart([&](ClientWaitPacket cw_packet)
		{
			num_clients = cw_packet.max_client;
			fprintf(stderr, "%d of %d players joined\n", cw_packet.client_joined, cw_packet.max_client);
		});
	fprintf(stderr, "All players connected, starting game\n");
	//loadingGuithread.join();
	//GUI::initializeGUI(window);
	
	Window::postprocessing = new FBO(-200.0f, 7500.0f);
	Window::bloom = new FBO(Window::width, Window::height);

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

				if (sheader->ui_open && !GUI::show_GUI) {
					GUI::ShowGUI(true);
				}
				else if (!sheader->ui_open && GUI::show_GUI){
					GUI::ShowGUI(false);
				}

				for (int i = 0; i < num_clients; i++) {
					GUI::scoreboard_data[i]=sheader->balance[i];
				}

				Window::postprocessing->draw(Window::width, Window::height, Window::view);
				for (int i = 0; i < sheader->num_models; i++)
				{
					ModelInfo model_info = model_arr[i];

					if (model_map.count(model_info.model_id) == 0) {
						model_map[model_info.model_id] = new Model(model_info.model);
					}
					else if (model_info.model != model_map[model_info.model_id]->getModelEnum()) {
						delete model_map[model_info.model_id];
						model_map[model_info.model_id] = new Model(model_info.model);
					}

					if (model_info.is_player) {
						GUI::player_pos[model_info.model] = ImVec2(
							model_info.parent_transform[3][0],
							model_info.parent_transform[3][2]);
					}
  
  				Model& curr_model = *model_map[model_info.model_id];

					//TODO Get rid of this lol, maybe make AnimSpeeds sent back from server?
					if (model_info.modelAnim == WALK || model_info.modelAnim == IDLE_WALK) {
						if (sheader->player_sprinting) {
							curr_model.anim_speed = 3.0f;
						}
						else {
							curr_model.anim_speed = 1.5f;
						}
					}
					else {
						curr_model.anim_speed = 1;
					}
					
					curr_model.setAnimationMode(model_info.modelAnim);
					curr_model.draw(model_info.parent_transform, Window::shadowShaderProgram);
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, Window::width, Window::height);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
				glBindFramebuffer(GL_FRAMEBUFFER, Window::bloom->sceneFBO);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				// Window::renderDepthMap();
				glCullFace(GL_BACK);
				for (int i = 0; i < sheader->num_models; i++)
				{
					ModelInfo model_info = model_arr[i];

					if (model_map.count(model_info.model_id) == 0) {
						model_map[model_info.model_id] = new Model(model_info.model);
					}
					else if (model_info.model != model_map[model_info.model_id]->getModelEnum()) {
							delete model_map[model_info.model_id];
							model_map[model_info.model_id] = new Model(model_info.model);
					}

					model_map[model_info.model_id]->setAnimationMode(model_info.modelAnim);

					model_map[model_info.model_id]->draw(view, Window::projection, model_info.parent_transform, Window::modelShader[model_info.model]);
				}

				
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glUseProgram(Window::blurShaderProgram);
				unsigned int amount = 2;
				bool horizontal = true, first_iteration = true;
				for (unsigned int i = 0; i < amount; i++)
				{
					glUniform1i(glGetUniformLocation(Window::blurShaderProgram, "image"), 0);
					glBindFramebuffer(GL_FRAMEBUFFER, FBO::pingpongFBO[horizontal]);
					glUniform1i(glGetUniformLocation(Window::blurShaderProgram, "horizontal"), horizontal);
					glBindTexture(GL_TEXTURE_2D, first_iteration ? FBO::colorBuffers[1] : FBO::pColorBuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
					Window::renderDepthMap();
					horizontal = !horizontal;
					if (first_iteration)
						first_iteration = false;
				}

				
				glUseProgram(0);
				glActiveTexture(GL_TEXTURE0);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glUseProgram(Window::finalShaderProgram);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, FBO::colorBuffers[0]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, FBO::pColorBuffers[!horizontal]);

				glUniform1i(glGetUniformLocation(Window::finalShaderProgram, "scene"), 0);
				glUniform1i(glGetUniformLocation(Window::finalShaderProgram, "bloomBlur"), 1);
				glUniform1i(glGetUniformLocation(Window::finalShaderProgram, "bloom"), 1);
				glUniform1f(glGetUniformLocation(Window::finalShaderProgram, "exposure"), 1.0f);
				Window::renderDepthMap();
				glUseProgram(0);
				

				free(sheader);
				free(model_arr);

				// TODO render minimap here using minimap_pos vectors
			});

		// Gets events, including input such as keyboard and mouse or window resizing
		glfwPollEvents();

		//IMGUI rendering
		GUI::renderUI();

		// Swap buffers.
		glfwSwapBuffers(window);

		//auto end_time = std::chrono::steady_clock::now();
		//long long elapsed_time_ms = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time).count();
		//printf("Client time between ticks: %lld us\n", elapsed_time_ms);
		//begin_time = end_time;
	}

	// destroy objects created
	Window::cleanUp();
	for (auto iter = model_map.begin(); iter != model_map.end(); iter++) {
		free(iter->second);
		//delete iter->second;
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