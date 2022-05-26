#include "main.h"
#include "UI/GUI.h"
#include "./Network/Client.h"
#include "./Network/NetworkPacket.h"
#include "ServerMain.cpp"
#include "ConfigReader.h"
#include <filesystem>
#include "Model.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <map>

#include "SoundEngine.h"

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
	glfwSetKeyCallback(window, InputManager::keyCallback);

	// Set cursor callback
	glfwSetCursorPosCallback(window, InputManager::CursorCallback);
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
		int width, height, nrComponents; 
		unsigned char* data = stbi_load(entry.path().string().c_str(), &width, &height, &nrComponents, 0);
		stbi_image_free(data);
		i++;
	}
	GUI::show_loading = false; 
}

void timing(std::chrono::time_point<std::chrono::steady_clock> &begin_time, std::string msg){
	//auto begin_time = std::chrono::steady_clock::now();
	auto end_time = std::chrono::steady_clock::now();
	long long elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time).count();
	printf("%s: %f ms\n", msg.c_str(), elapsed_time/1000.f);
	begin_time = end_time;
}

void preload_assets(GLFWwindow* window) 
{
	Model tmp; 

	int size = SENTINEL_END - SENTINEL_BEGIN; // implicit + 2 from (progress = 1) and excess (progress++)

	float progress = 1; 
	bool flip_image = true; // variable use to flip the image
	std::atomic_bool spawned(false);
	std::atomic_bool done(false);
	
	for (ModelEnum i = static_cast<ModelEnum>(SENTINEL_BEGIN + 1); i < SENTINEL_END;) {
		auto begin_time = std::chrono::steady_clock::now();

		flip_image = GUI::renderProgressBar(progress / size, window, flip_image);

		if (!spawned.load(std::memory_order_relaxed))
		{
			spawned.store(true, std::memory_order_relaxed);
			std::thread worker([&]()
			{
				Model::load_scene(i);
				done.store(true, std::memory_order_relaxed);
			});
			worker.detach();	
		}

		if (done.load(std::memory_order_relaxed))
		{
			tmp = Model(i);
			progress++;
		
			i = static_cast<ModelEnum>(i + 1);
			spawned.store(false, std::memory_order_relaxed);
			done.store(false, std::memory_order_relaxed);
		}

		auto end_time = std::chrono::steady_clock::now();
		int elapsed_time = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count());
		Sleep(std::max(130 - elapsed_time, 0));
	}
	flip_image = GUI::renderProgressBar(progress / size, window, flip_image);
	GUI::initializeImage();
	progress++;
	flip_image = GUI::renderProgressBar(progress / size, window, flip_image);
}

int main(int argc, char* argv[])
{
	if (argc > 1 && (std::string(argv[1]) == "server-build")) { //TODO Add Seperate Project
		ServerMain();
		return 0;
	}
	
	// Create the GLFW window.
	GLFWwindow* window = Window::createWindow(1920, 1080);
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

	// display loading bar
	GUI::show_loading = true;
	
	// initialize IMGUI 
	GUI::initializeGUI(window);
	GUI::initializeLoadingImage();

	// Initialize sound engine
	SoundEngine sound_engine{};
	sound_engine.Init();

	// preload models and other assets
	preload_assets(window);
	
	// Initialize network client interface
	static std::unordered_map<std::string, std::string> client_config = ConfigReader::readConfigFile("client.cfg");
	Client* client = new Client(client_config["server_address"].c_str(), DEFAULT_PORT);

	// client-side wait until all clients are connected
	sound_engine.PlayMusic(MUSIC_MENU);
	GUI::renderWaitingClient(1, 1);
	int num_clients = 4;
	client->syncGameReadyToStart([&](ClientWaitPacket cw_packet)
		{
			num_clients = cw_packet.max_client;
			fprintf(stderr, "%d of %d players joined\n", cw_packet.client_joined, cw_packet.max_client);
			GUI::renderWaitingClient(cw_packet.client_joined, cw_packet.max_client); 
		});
	fprintf(stderr, "All players connected, starting character selection\n");
	
	// character selection 
	GUI::char_selections = client->syncCharacterSelection(num_clients,[&](ServerCharacterPacket recv_packet) 
	{
		std::unordered_set<ModelEnum> char_selections{recv_packet.current_char_selections, recv_packet.current_char_selections + num_clients};
		ModelEnum res = GUI::renderCharacterSelection(char_selections, recv_packet.client_idx);
		
		ClientCharacterPacket out_packet;
		out_packet.character = res;
		return out_packet;
	});
	fprintf(stderr, "All players selected character, starting game\n");

	// butter butter magic
	Window::postprocessing = new FBO(-200.0f, 7500.0f);
	Window::bloom = new FBO(Window::width, Window::height);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	// "globals" for the duration of the client main loop
	int status = 1;
	std::map<uintptr_t, Model*> model_map;
	glm::vec3 eye_offset = glm::vec3(0,30,30); //TODO no magic number :,(

	sound_engine.Play(SFX_AMBIENCE);
	sound_engine.PlayMusic(MUSIC_DAY_1, false);

	// Loop while GLFW window should stay open and server hasn't closed connection
	while (!glfwWindowShouldClose(window) && status > 0)
	{
		// adjust camera direction (player's forward direction depends on camera)
		const glm::vec2 mouse_delta = InputManager::GetCursorDelta();
		glm::mat3 rot_x = util_RotateAroundAxis(-mouse_delta[0] * InputManager::camera_speed, glm::vec3{0,1,0});
		auto rot_y = glm::mat3(1);
		if (eye_offset[1] < 40.f || mouse_delta[1] <= 0) { //TODO no magic numbers :^(
			const glm::vec3 right_axis = -glm::cross(glm::vec3{0,1,0},eye_offset);
			rot_y = util_RotateAroundAxis(mouse_delta[1] * InputManager::camera_speed, glm::normalize(right_axis));
		}
		eye_offset = rot_x * rot_y * eye_offset;
		eye_offset = glm::normalize(eye_offset) * InputManager::camera_dist;
		if (eye_offset[1] < 15) eye_offset[1] = 15;
		eye_offset = eye_offset * (eye_offset[1]/30.f);
		
		// after moving camera, move the player
		ClientPacket out_packet;
		const glm::vec2 movement = InputManager::getLastMovement();
		const float degrees = atan2f(eye_offset[0], eye_offset[2]);
		const glm::mat2 trans = util_Rotate2D(degrees);
		out_packet.movement = trans * movement;
		
		out_packet.lastCommand = InputManager::getLastCommand();

		InputManager::reset();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		status = client->syncWithServer(&out_packet, sizeof(out_packet), [&](char* recv_buf, size_t recv_len) {
			// deserialize incoming packet into structs
			ServerHeader* sheader;
			ModelInfo* model_arr;
			SoundInfo* sound_arr;
			PendingDeleteInfo* pending_arr;
			serverDeserialize(recv_buf, &sheader, &model_arr, &sound_arr, &pending_arr);

			//Rendering Code
			const glm::mat4 player_transform = sheader->player_transform;
			const glm::vec3 player_pos = glm::vec3(player_transform[3][0], player_transform[3][1], player_transform[3][2]) / player_transform[3][3];
			const glm::vec3 eye_pos = player_pos + eye_offset;	// TODO implement angle.
			const glm::vec3 look_at_point = player_pos; // The point we are looking at.
			const glm::mat4 view = glm::lookAt(eye_pos, look_at_point, Window::upVector);

			std::vector<glm::vec3> point_light_pos; // TODO ask cynthia <--- used to get players positions for point lights in night phase -cynthia

			// check if need to trigger client-side winning sequence
			if (sheader->time_remaining_seconds <= 0)
			{
				GUI::GUI_show_winning = true;
			}

			// update scoreboard
			for (int i = 0; i < num_clients; i++) {
				GUI::scoreboard_data[i] = sheader->balance[i];
			}

			// update stamina
			GUI::stamina_percent = sheader->stamina_bar;

			// update timer
			GUI::setTimer(static_cast<float>(1 - (sheader->time_remaining_seconds / sheader->time_max_seconds)), sheader->time_remaining_seconds);
			char strbuf[16];
			int rem_s = static_cast<int>(sheader->time_remaining_seconds);
			sprintf(strbuf, "%02d:%02d", rem_s / 60, rem_s % 60);
			GUI::GUI_timer_string = std::string(strbuf);

			// show NPC UI page
			GUI::ShowGUI(sheader->ui_open);

			// play sounds
			for (int i = 0; i < sheader->num_sounds; i++) {
				const SoundInfo sound_info = sound_arr[i];

				//TODO Add location of sound so we have panning
				sound_engine.Play(sound_info.sound);
			}

			Window::postprocessing->draw(Window::width, Window::height, sheader->time_remaining_seconds, Window::view);
			for (int i = 0; i < sheader->num_models; i++)
			{
				ModelInfo& model_info = model_arr[i];

				// insert new model into map
				if (model_map.count(model_info.model_id) == 0) {
					model_map[model_info.model_id] = new Model(model_info.model);
				}
				else if (model_info.model != model_map[model_info.model_id]->getModelEnum()) {
					delete model_map[model_info.model_id];
					model_map[model_info.model_id] = new Model(model_info.model);
				}
				Model& curr_model = *model_map[model_info.model_id];

				// FBO and minimap needs player data
				if (model_info.is_player) {
					GUI::player_pos[model_info.model - CHAR_BUMBUS] = ImVec2(
						model_info.parent_transform[3][0],
						model_info.parent_transform[3][2]);
					point_light_pos.push_back(glm::vec3(model_info.parent_transform[3]) + glm::vec3(0.0f, 3.0f, 0.0f));
				}

				// set player animation speed
				//TODO Get rid of this lol, maybe make AnimSpeeds sent back from server?
				if (curr_model.hasAni) {
					if (model_info.modelAnim == WALK || model_info.modelAnim == IDLE_WALK) {
						if (sheader->player_sprinting) {
							curr_model.anim_speed = 3.0f;
						}
						else {
							curr_model.anim_speed = 1.5f;
						}
					}

					else {
						curr_model.anim_speed = 1.0f;
					}
				}
				curr_model.setAnimationMode(model_info.modelAnim);

				// get light for NPC and golden eggplant
				if (model_info.model == CHAR_NPC || model_info.model == VEG_GOLDEN_EGGPLANT) {
					glm::vec3 pos = glm::vec3(model_info.parent_transform[3]) + glm::vec3(0.0f, 6.0f, 0.0f);

					// if eggplant is dropped, point light wont show. set minimum to 1.
					if (pos.y == 0.0f) { pos.y = 1.0f; }
					point_light_pos.push_back(pos);
				}

				// rotating particles towards players
				else if (model_info.model == INDICATOR_WATER || model_info.model == INDICATOR_FERTILIZER || model_info.model == PARTICLE_GLOW) {
					glm::mat4 tempTransform = model_info.parent_transform;
					glm::vec3 plot_pos = glm::vec3(tempTransform[3][0], tempTransform[3][1], tempTransform[3][2]);
					glm::mat4 rotation;
					
					if (model_info.model == PARTICLE_GLOW)
					{
						rotation = glm::mat4(1);
					}
					else 
					{
						rotation = glm::lookAt(glm::vec3(plot_pos.x, 0, -plot_pos.z), glm::vec3(eye_pos.x, 0, -eye_pos.z), glm::vec3(0, 1, 0));
					}

					//CONCAT CODE
					rotation[3][0] = tempTransform[3][0];
					rotation[3][1] = tempTransform[3][1];
					rotation[3][2] = tempTransform[3][2];

					model_info.parent_transform = rotation;
				}

				// Do not draw shadow for the big sky dome
				if (model_info.model != WORLD_DOME) {
					curr_model.draw(model_info.parent_transform, Window::shadowShaderProgram);
				}
			}
		
			// for point lights in night phase
			FBO::playerPos = point_light_pos;

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, Window::width, Window::height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glBindFramebuffer(GL_FRAMEBUFFER, Window::bloom->sceneFBO);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_BACK);
			for (int i = 0; i < sheader->num_models; i++) // TODO ask Danica planting bandaid nani
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

			FBO::bloomBlur(Window::blurShaderProgram);
			FBO::finalDraw(Window::finalShaderProgram);

			for (int i = 0; i < sheader->num_pending_delete; i++) {
				const uintptr_t pending_delete_id = pending_arr[i].model_id;
				fprintf(stderr, "Client: deleting model with ID %lld\n", pending_delete_id);

				Model* model_to_delete = model_map[pending_delete_id];
				model_map.erase(pending_delete_id);
				delete model_to_delete;
			}

			free(sheader);
			free(model_arr);
			free(sound_arr);
			free(pending_arr);
		});

		// Gets events, including input such as keyboard and mouse or window resizing
		glfwPollEvents();

		//IMGUI rendering
		if (GUI::GUI_show_winning) {
			GUI::renderWinningScene();
		} else {
			GUI::renderUI();
		}


		// Swap buffers.
		glfwSwapBuffers(window);
	}

	sound_engine.DeInit();

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