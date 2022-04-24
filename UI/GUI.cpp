#include "GUI.h"
#include <iostream>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
vector<GUIImage> GUI::rack_images_list;
vector<GUIImage> GUI::icon_images_list;
GUIImage GUI::score_background; 
int GUI::rack_image_idx; 
string GUI::picture_dir;
float GUI::rack_size_ratio; 
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
	picture_dir = "./UI/Pictures";
	rack_size_ratio = 0.5f;
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
	//IMGUI rendering
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
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
	ImGui::SetNextWindowSize(ImVec2(score_background.my_image_width * 0.5f, score_background.my_image_height * 0.5f));
	ImGui::Begin("ScoreBoard_bg", NULL, window_flags);
	ImGui::Image((void*)(intptr_t)score_background.my_image_texture , ImVec2(score_background.my_image_width*0.5f, score_background.my_image_height*0.5f));
	ImGui::End();
	ImGui::SetNextWindowPos(ImVec2(80, 80), 0, ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(score_background.my_image_width * 0.5f, score_background.my_image_height * 0.5f));
	ImGui::Begin("ScoreBoard_content", NULL, window_flags);
	for (vector<GUIImage>::iterator it = icon_images_list.begin(); it != icon_images_list.end(); it++) {
		GUIImage image = *(it);
		ImGui::Image((void*)(intptr_t)image.my_image_texture, ImVec2(image.my_image_width, image.my_image_height));
		ImGui::SameLine();
		ImGui::Text("2000");
	}
	ImGui::End(); 

	//show the sale page
	if(show_GUI == true) {		
		// etc.
		bool open_ptr = true; 
		ImGui::SetNextWindowSize(ImVec2(rack_images_list[0].my_image_width * rack_size_ratio, rack_images_list[0].my_image_height * rack_size_ratio));
		ImGui::Begin("My Name is Window, IMGUI Window", &open_ptr, window_flags);
		//ImGui::Text("Hello there adeventurer!");
	
		if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
			if(rack_image_idx<rack_images_list.size()-1)
				rack_image_idx++;
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
			if(rack_image_idx>0)
				rack_image_idx--; 
		}
		GUIImage image = rack_images_list[rack_image_idx];
		ImGui::Image((void*)(intptr_t)image.my_image_texture, ImVec2(image.my_image_width*rack_size_ratio, image.my_image_height*rack_size_ratio));//image.my_image_width, image.my_image_height));
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
	for (auto& entry : fs::directory_iterator(rack_dir)) {
		std::cout << entry.path() << std::endl;
		GUIImage image;
		const char* epath = entry.path().string().c_str(); 
		bool ret = LoadTextureFromFile(epath, &(image.my_image_texture),
			&(image.my_image_width), &(image.my_image_height));
		rack_images_list.push_back(image);
	}
	rack_image_idx = 0;

	const char* icon_dir = (picture_dir + string("/icon")).c_str();
	for (auto& entry : fs::directory_iterator(icon_dir)) {
		std::cout << entry.path() << std::endl;
		GUIImage image;
		const char* epath = entry.path().string().c_str();
		bool ret = LoadTextureFromFile(epath, &(image.my_image_texture),
			&(image.my_image_width), &(image.my_image_height));
		icon_images_list.push_back(image);
	}

	const char* score_bg_path = (picture_dir + string("/score_background.png")).c_str();
	LoadTextureFromFile(score_bg_path, &(score_background.my_image_texture),
		&(score_background.my_image_width), &(score_background.my_image_height));
}
