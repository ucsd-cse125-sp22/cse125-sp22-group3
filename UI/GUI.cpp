#include "GUI.h"
#include <iostream>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
GUIImage GUI::rack_images_list[NUM_RACK_IMG];
GUIImage GUI::icon_images_list[NUM_ICON];
GUIImage GUI::score_background; 
GUIImage GUI::loading_bg[NUM_LOAD_IMG];

int GUI::rack_image_idx; 
string GUI::picture_dir;
float GUI::rack_size_ratio; 
GLFWwindow* GUI::my_window;

// Initialized IMGUI, check the version of glfw and initializes imgui accordingly, should be called before the main loop
void GUI::initializeGUI(GLFWwindow* window) {
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	/*IMGUI Initialize*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImFont* font1 = io.Fonts->AddFontFromFileTTF("./UI/fonts/PlayfairDisplay-VariableFont_wght.ttf", 36.0f);
	picture_dir = "./UI/Pictures";
	rack_size_ratio = 0.5f;
	my_window = window;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.Fonts->AddFontFromFileTTF("font.ttf", 18.0f);
	
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	/*END of IMGUI initialize*/

}

/**
* Render the UI compenents, should be called within the mainloop
* RenderUI need to be called after clear Opengl buffer and before swapbuffer
* TODO: support interaction and multiple UI. now only render one ui, 
*/
bool GUI::renderUI(bool show_GUI) {
	//get current main window size and calculate the ratio for dynamic resizing
	int width, height; 
	glfwGetWindowSize(my_window, &width, &height);
	float display_ratio = 0.2f * width / 1280; 
	//IMGUI rendering
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	ImGui::GetWindowSize(); 

	if (ImGui::IsKeyPressed(ImGuiKey_K)) {
		show_GUI = !show_GUI;
		rack_image_idx = 0;

	}
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove; 
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//show scoreboard
	ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
	ImGui::Begin("ScoreBoard_bg", NULL, window_flags);
	ImGui::Image((void*)(intptr_t)score_background.my_image_texture , ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
	ImGui::End();
	ImGui::SetNextWindowPos(ImVec2(190*display_ratio, 190*display_ratio), 0, ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
	ImGui::Begin("ScoreBoard_content", NULL, window_flags);

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(51, 48, 49, 255));
	
	for (int i = 0; i < NUM_ICON; i++) {
		GUIImage image = icon_images_list[i];
		ImGui::Image((void*)(intptr_t)image.my_image_texture, ImVec2(image.my_image_width * display_ratio, image.my_image_height * display_ratio));
		ImGui::SameLine();
		ImGui::Text("2000");
	}
	ImGui::PopStyleColor();
	ImGui::End(); 

	//show the sale page
	if(show_GUI == true) {		
		if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
			if (rack_image_idx < NUM_RACK_IMG - 1)
				rack_image_idx++;
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
			if (rack_image_idx > 0)
				rack_image_idx--;
		}
		// etc.
		bool open_ptr = true;
		GUIImage image = rack_images_list[rack_image_idx];

		ImVec2 size = ImVec2(image.my_image_width * display_ratio, image.my_image_height * display_ratio);
		ImGui::SetNextWindowSize(ImVec2(width,height));
		ImGui::Begin("Sale GUI", &open_ptr, window_flags);
		ImGui::SetCursorPos((ImGui::GetContentRegionAvail() - size) * 0.5f);
		//ImGui::Text("Hello there adeventurer!");	
		ImGui::Image((void*)(intptr_t)image.my_image_texture, size);//image.my_image_width, image.my_image_height));
		//ImGui::Text("image demension:%dx%d", image.my_image_width, image.my_image_height);
		ImGui::End();
	}

	

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	return show_GUI;
}

// Cleanup IMGUI, should be called after the mainloop
void GUI::cleanUp() {
	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

// Simple helper function to load an image into a OpenGL texture with common settings
bool GUI::LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (stbi_failure_reason())
		std::cout << stbi_failure_reason();
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

void GUI::initializeImage() {
	const char* rack_dir = (picture_dir + string("/rack")).c_str(); 
	int i = 0;
	for (auto& entry : fs::directory_iterator(rack_dir)) {
		std::cout << entry.path() << std::endl;
		GUIImage* image = &(rack_images_list[i]);
		const char* epath = entry.path().string().c_str(); 
		bool ret = LoadTextureFromFile(epath, &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		i++;
	}
	rack_image_idx = 0;
	i = 0; 
	const char* icon_dir = (picture_dir + string("/icon")).c_str();
	for (auto& entry : fs::directory_iterator(icon_dir)) {
		std::cout << entry.path() << std::endl;
		GUIImage* image = &(icon_images_list[i]);
		const char* epath = entry.path().string().c_str();
		bool ret = LoadTextureFromFile(epath, &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		image->my_image_width *= 2.0f;
		image->my_image_height *= 2.0f; 
		i++;
	}

	const char* score_bg_path = (picture_dir + string("/score_background.png")).c_str();
	LoadTextureFromFile(score_bg_path, &(score_background.my_image_texture),
		&(score_background.my_image_width), &(score_background.my_image_height));

	
}

//bool GUI::renderLoadScene(GLFWwindow* window) {
//	glfwMakeContextCurrent(window);
//	glfwSwapInterval(0); // Enable vsync
//	initializeGUI(window);
//	
//	const char* loading_bg_path = (picture_dir + string("/loading_background.png")).c_str();
//	LoadTextureFromFile(loading_bg_path, &(loading_bg.my_image_texture), &(loading_bg.my_image_width), &(loading_bg.my_image_height));
//	while (!glfwWindowShouldClose(window) && show_loading) {
//		glfwPollEvents();
//		ImGui_ImplOpenGL3_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//		ImGui::NewFrame();
//		int width, height;
//		glfwGetWindowSize(window, &width, &height);
//		float display_ratio = 0.2f * width / 1280;
//
//
//		ImGui::SetNextWindowSize(ImVec2(loading_bg.my_image_width * display_ratio, loading_bg.my_image_height * display_ratio));
//		ImGuiWindowFlags window_flags = 0;
//		window_flags |= ImGuiWindowFlags_NoBackground;
//		window_flags |= ImGuiWindowFlags_NoTitleBar;
//		window_flags |= ImGuiWindowFlags_NoResize;
//		window_flags |= ImGuiWindowFlags_NoMove;
//		window_flags |= ImGuiWindowFlags_NoScrollbar;
//
//		ImGui::Begin("Loading Screen", NULL, window_flags);
//		ImGui::Image((void*)(intptr_t)loading_bg.my_image_texture, ImVec2(loading_bg.my_image_width * display_ratio, loading_bg.my_image_height * display_ratio));
//		ImGui::End();
//		// Rendering
//		int display_w, display_h;
//		glfwGetFramebufferSize(window, &display_w, &display_h);
//		glViewport(0, 0, display_w, display_h);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		ImGui::Render();
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//		glfwSwapBuffers(window);
//	}
//	//ImGui_ImplOpenGL3_Shutdown();
//	//ImGui_ImplGlfw_Shutdown();
//	//ImGui::DestroyContext();
//	return true; 
//}
bool GUI::renderLoadScene() {
	const char* load_dir = (picture_dir + string("/loading")).c_str();
	int i = 0; 
	for (auto& entry : fs::directory_iterator(load_dir)) {
		std::cout << entry.path() << std::endl;
		GUIImage* image = &(loading_bg[i]);
		const char* epath = entry.path().string().c_str();
		bool ret = LoadTextureFromFile(epath, &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		i++;
	}
	int idx = 0; 
	bool increase = true; 
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoScrollbar;

	while (!glfwWindowShouldClose(my_window) && show_loading) {
		
		if ((idx == 0 && !increase)|| (idx == 6 && increase)) {
			increase = !increase; 
		}

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		int width, height;
		glfwGetWindowSize(my_window, &width, &height);
		float display_ratio = 0.4f * width / 1280;
		ImVec2 size = ImVec2(loading_bg[idx].my_image_width * display_ratio, loading_bg[idx].my_image_height * display_ratio);
		ImGui::SetNextWindowSize(size);
		ImGui::SetNextWindowPos((ImVec2(width, height) - size) * 0.5f);
		ImGui::Begin("Loading Screen", NULL, window_flags);
		ImGui::Image((void*)(intptr_t)loading_bg[idx].my_image_texture, ImVec2(loading_bg[idx].my_image_width * display_ratio, loading_bg[idx].my_image_height * display_ratio));
		ImGui::End();

		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(my_window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(my_window);

		if (increase) {
			idx++;
		}else {
			idx--; 
		}
		this_thread::sleep_for(chrono::milliseconds(300));
	}
	return true; 
}
