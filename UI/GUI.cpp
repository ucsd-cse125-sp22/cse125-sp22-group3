#include "GUI.h"
#include "../util.h"
#include <iostream>
//#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;
#include <filesystem>
namespace fs = std::filesystem;

bool GUI::show_GUI = false;
GUIImage GUI::rack_images_list[NUM_RACK_IMG];
GUIImage GUI::icon_images_list[NUM_ICON];
GUIImage GUI::score_background; 
GUIImage GUI::loading_background;

GUIImage GUI::loading_bg[NUM_LOAD_IMG];
GUIImage GUI::chase_images_list[NUM_CHASE_IMG];
int GUI::scoreboard_data[NUM_ICON];

float GUI::display_ratio;
int GUI::window_height;
int GUI::window_width;

int GUI::rack_image_idx; 
std::string GUI::picture_dir;
GLFWwindow* GUI::my_window;
ImVec2 GUI::player_pos[4];

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

void GUI::updateDisplayRatio(int width, int height) {
	display_ratio = 0.2f * width / 1280;
	window_width = width;
	window_height = height; 
}

/**
* Render the UI compenents, should be called within the mainloop
* RenderUI need to be called after clear Opengl buffer and before swapbuffer
* TODO: support interaction and multiple UI. now only render one ui, 
*/
bool GUI::renderUI() {
	

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	//if (ImGui::IsKeyPressed(ImGuiKey_K)) {
	//	GUI_showNPCui = !GUI_showNPCui;
	//	rack_image_idx = 0;
	//}
	
	/* build scoreboard */
	ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
	ImGui::Begin("ScoreBoard_bg", NULL, TRANS_WINDOW_FLAG);
	ImGui::Image((void*)(intptr_t)score_background.my_image_texture , ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
	ImGui::End();
	ImGui::SetNextWindowPos(ImVec2(190*display_ratio, 190*display_ratio), 0, ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
	ImGui::Begin("ScoreBoard_content", NULL, TRANS_WINDOW_FLAG);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(51, 48, 49, 255));
	
	for (int i = 0; i < NUM_ICON; i++) {
		GUIImage image = icon_images_list[i];
		ImGui::Image((void*)(intptr_t)image.my_image_texture, ImVec2(image.my_image_width * display_ratio, image.my_image_height * display_ratio));
		ImGui::SameLine();
		ImGui::Text("%d",scoreboard_data[i]);
	}
	ImGui::PopStyleColor();
	ImGui::End(); 
	/* end of scoreboard */

	/* build the seed sale page */
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
		ImGui::SetNextWindowSize(ImVec2(window_width,window_height));
		ImGui::Begin("Sale GUI", &open_ptr, TRANS_WINDOW_FLAG);
		ImGui::SetCursorPos((ImGui::GetContentRegionAvail() - size) * 0.5f);
		//ImGui::Text("Hello there adeventurer!");	
		ImGui::Image((void*)(intptr_t)image.my_image_texture, size);//image.my_image_width, image.my_image_height));
		//ImGui::Text("image demension:%dx%d", image.my_image_width, image.my_image_height);
		ImGui::End();
	}
	/*end of seed sale page*/
	createMiniMap(); 
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
	const char* rack_dir = (picture_dir + std::string("/rack")).c_str(); 
	int i = 0;
	for (auto& entry : fs::directory_iterator(rack_dir)) {
		//std::cout << entry.path() << std::endl;
		GUIImage* image = &(rack_images_list[i]);
		const char* epath = entry.path().string().c_str(); 
		bool ret = LoadTextureFromFile(epath, &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		i++;
	}
	rack_image_idx = 0;
	i = 0; 
	const char* icon_dir = (picture_dir + std::string("/icon")).c_str();
	for (auto& entry : fs::directory_iterator(icon_dir)) {
		//std::cout << entry.path() << std::endl;
		GUIImage* image = &(icon_images_list[i]);
		const char* epath = entry.path().string().c_str();
		bool ret = LoadTextureFromFile(epath, &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		image->my_image_width *= 2.0f;
		image->my_image_height *= 2.0f;
		i++;
	}

	const char* score_bg_path = (picture_dir + std::string("/score_background.png")).c_str();
	LoadTextureFromFile(score_bg_path, &(score_background.my_image_texture),
		&(score_background.my_image_width), &(score_background.my_image_height));

	const char* loading_bg_path = (picture_dir + std::string("/loading_background.png")).c_str();
	LoadTextureFromFile(loading_bg_path, &(loading_background.my_image_texture),
		&(loading_background.my_image_width), &(loading_background.my_image_height));


	i = 0;
	const char* chase_dir = (picture_dir + std::string("/chasing")).c_str();
	for (auto& entry : fs::directory_iterator(chase_dir)) {
		//std::cout << entry.path() << std::endl;
		GUIImage* image = &(chase_images_list[i]);
		const char* epath = entry.path().string().c_str();
		bool ret = LoadTextureFromFile(epath, &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		image->my_image_width *= 2.0f;
		image->my_image_height *= 2.0f;
		i++;
	}
}

void GUI::initializeLoadingImage() {
	const char* load_dir = (picture_dir + std::string("/loading")).c_str();
	int i = 0;
	for (auto& entry : fs::directory_iterator(load_dir)) {
		//std::cout << entry.path() << std::endl;
		GUIImage* image = &(loading_bg[i]);
		const char* epath = entry.path().string().c_str();
		bool ret = LoadTextureFromFile(epath, &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		i++;
	}
}

bool GUI::renderLoadScene(GLFWwindow* window) {
	initializeLoadingImage();

	glClearColor(255.f / 255, 222.f / 255, 194.f / 255, 1.0);
	int idx = 0; 
	bool increase = true; 
	ImGuiWindowFlags trans_win_flags = 0;
	trans_win_flags |= ImGuiWindowFlags_NoBackground;
	trans_win_flags |= ImGuiWindowFlags_NoTitleBar;
	trans_win_flags |= ImGuiWindowFlags_NoResize;
	trans_win_flags |= ImGuiWindowFlags_NoMove;
	trans_win_flags |= ImGuiWindowFlags_NoScrollbar;

	while (!glfwWindowShouldClose(window)) {
		
		if ((idx == 0 && !increase)|| (idx == 6 && increase)) {
			increase = !increase; 
		}

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		
		ImVec2 size = ImVec2(loading_bg[idx].my_image_width * display_ratio * 2, loading_bg[idx].my_image_height * display_ratio * 2);
		ImGui::SetNextWindowSize(size);
		ImGui::SetNextWindowPos((ImVec2(window_width, window_height) - size) * 0.5f);
		ImGui::Begin("Loading Screen", NULL, trans_win_flags);
		ImGui::Image((void*)(intptr_t)loading_bg[idx].my_image_texture, ImVec2(loading_bg[idx].my_image_width * display_ratio, loading_bg[idx].my_image_height * display_ratio));
		ImGui::End();

		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		

		//handling the last image; 
		if (idx == 7) {
			break; 
		}else if (!show_loading && idx == 6) {
			idx++;
			continue; 
		}

		if (increase) {
			idx++;
		}else {
			idx--; 
		}

	}
	glClearColor(212.f / 255, 242.f / 255, 148.f / 255, 1.0);
	return true; 
}

//bool GUI::renderLoadScene(GLFWwindow* window) {
//	glClearColor(255.f / 255, 222.f / 255, 194.f / 255, 1.0);
//	int idx = 0; 
//	bool increase = true; 
//	ImGuiWindowFlags window_flags = 0;
//	window_flags |= ImGuiWindowFlags_NoBackground;
//	window_flags |= ImGuiWindowFlags_NoTitleBar;
//	window_flags |= ImGuiWindowFlags_NoResize;
//	window_flags |= ImGuiWindowFlags_NoMove;
//	window_flags |= ImGuiWindowFlags_NoScrollbar;
//
//	while (!glfwWindowShouldClose(my_window)) {
//		
//		if ((idx == 0 && !increase)|| (idx == 6 && increase)) {
//			increase = !increase; 
//		}
//
//		glfwPollEvents();
//
//		ImGui_ImplOpenGL3_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//		ImGui::NewFrame();
//
//		int width, height;
//		glfwGetWindowSize(my_window, &width, &height);
//		float display_ratio = 0.4f * width / 1280;
//		ImVec2 size = ImVec2(loading_bg[idx].my_image_width * display_ratio, loading_bg[idx].my_image_height * display_ratio);
//		ImGui::SetNextWindowSize(size);
//		ImGui::SetNextWindowPos((ImVec2(width, height) - size) * 0.5f);
//		ImGui::Begin("Loading Screen", NULL, window_flags);
//		ImGui::Image((void*)(intptr_t)loading_bg[idx].my_image_texture, ImVec2(loading_bg[idx].my_image_width * display_ratio, loading_bg[idx].my_image_height * display_ratio));
//		ImGui::End();
//
//		// Rendering
//		int display_w, display_h;
//		glfwGetFramebufferSize(my_window, &display_w, &display_h);
//		glViewport(0, 0, display_w, display_h);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		ImGui::Render();
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//		glfwSwapBuffers(my_window);
//		
//		this_thread::sleep_for(chrono::milliseconds(300));
//
//		//handling the last image; 
//		if (idx == 7) {
//			break; 
//		}else if (!show_loading && idx == 6) {
//			idx++;
//			continue; 
//		}
//
//		if (increase) {
//			idx++;
//		}else {
//			idx--; 
//		}
//
//	}
//	glClearColor(212.f / 255, 242.f / 255, 148.f / 255, 1.0);
//	return true; 
//}
namespace ImGui {

	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = size_arg;
		size.x -= style.FramePadding.x * 2;

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		// Render
		const float circleStart = size.x * 0.7f;
		const float circleEnd = size.x;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

		const float t = g.Time;
		const float r = size.y / 2;
		const float speed = 1.5f;

		const float a = speed * 0;
		const float b = speed * 0.333f;
		const float c = speed * 0.666f;

		const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
		const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
		const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
		return true;
	}

}
bool GUI::renderProgressBar(float percent, GLFWwindow* window, bool flip_image) {
	glClearColor(255.f / 255, 222.f / 255, 194.f / 255, 1.0);
	glfwPollEvents();
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	ImVec2 size = ImVec2(1200 * display_ratio * 5, 18 * display_ratio * 5);
	ImGui::SetNextWindowPos((ImVec2(window_width, window_height) - size) * 0.5f);
	ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
	ImGui::Begin("Progress Indicators", NULL, TRANS_WINDOW_FLAG);

	const ImU32 col = IM_COL32(245.f, 61.f,119.f, 255);//ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	const ImU32 bg = IM_COL32(227.f, 188.f, 208.f, 255); //ImGui::GetColorU32(ImGuiCol_Button);

	//ImGui::Spinner("##spinner", 15, 6, col);
	int idx = flip_image ? 1 : 0;
	ImGui::Image((void*)(intptr_t)chase_images_list[idx].my_image_texture, \
						ImVec2(chase_images_list[idx].my_image_width * display_ratio,\
							   chase_images_list[idx].my_image_height * display_ratio));
	ImGui::Text("Loading: %d %c...", (int)(percent * 100), '%');
	ImGui::BufferingBar("##buffer_bar", percent, size, bg, col);
	ImGui::End();

	// Rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
	return !flip_image;
}
bool GUI::ShowGUI(bool show)
{
	show_GUI = show;
	if (show == true) {
		rack_image_idx = 0;
	}
	return show;
}
/**
	this method construct the minimap on the corner of the screen base on the player locations
	only add the require elements for minimap but not actually render it
*/
void GUI::createMiniMap() {
	//get size of minimap 
	float world_dim = 500.0f;
	int width = 1200;
	int height = 1200;
	float padding = 32.0f;
	//size of minimap 
	ImVec2 size = ImVec2(width * display_ratio, height * display_ratio);

	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(ImVec2(padding, window_height - padding), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	//ImVec2 center = ImVec2(padding + size.x / 2, window_height - padding - size.y / 2); // center of minimap
	ImVec2 center = ImVec2(size.x / 2.0f, size.y /2.0f); // center of minimap

	bool bptr;
	ImGui::Begin("MiniMap", &bptr, MINI_MAP_FLAG);
	//place all player's icon:
	for (int i = 0; i < 4; i++) {
		GUIImage image = icon_images_list[i];
		//TODO convert the pos into relative pos in minimap
		ImVec2 icon_size = ImVec2(image.my_image_width * display_ratio, image.my_image_height * display_ratio);

		ImVec2 relative_pos = ImVec2(player_pos[i].x / world_dim * size.x- icon_size.x * 0.5f, player_pos[i].y / world_dim * size.y - icon_size.y * 0.5f); 
		//size of icon 
		ImGui::SetCursorPos(relative_pos + center);
		ImGui::Image((void*)(intptr_t)image.my_image_texture, icon_size);
	}
	ImGui::End();
}




