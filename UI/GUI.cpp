#include "GUI.h"
#include "../util.h"
#include <iostream>
//#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;
#include <filesystem>
namespace fs = std::filesystem;

GUIImage GUI::rack_images_list[NUM_RACK_IMG];
GUIImage GUI::icon_images_list[NUM_ICON];
GUIImage GUI::score_background; 
GUIImage GUI::loading_background;
GUIImage GUI::minimap_background;


GUIImage GUI::loading_bg[NUM_LOAD_IMG];
GUIImage GUI::chase_images_list[NUM_CHASE_IMG];
int GUI::scoreboard_data[NUM_ICON];
GUIImage GUI::fish_images_list[NUM_FISH_IMG];

float GUI::display_ratio;
int GUI::window_height;
int GUI::window_width;
bool GUI::prev_show_sale_ui;
float GUI::timer_percent;



int GUI::rack_image_idx; 
std::string GUI::picture_dir;
GLFWwindow* GUI::my_window;
ImVec2 GUI::player_pos[4];
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

	bool ReverseBufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
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
		const float circleStart = size.y;
		const float circleEnd = size.y;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(bb.Max.x, pos.y + circleStart), bg_col, ImDrawFlags_RoundCornersAll);
		window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y + circleStart * (1 - value)), ImVec2(bb.Max.x, pos.y + circleStart), fg_col, ImDrawFlags_RoundCornersAll);
		return true;
	}
	ImVec2 Spinner(const char* label, float radius, int thickness, float ratio, const ImU32& color) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return ImVec2(0, 0);

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return ImVec2(0, 0);

		// Render
		window->DrawList->PathClear();

		int num_segments = 30;
		int start = 1; //abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

		const float a_min = IM_PI * 1.5f; // IM_PI * 2.0f * ((float)start) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius + thickness, pos.y + radius + thickness);

		for (int i = 0; i <= num_segments * ratio; i++) {
			const float a = a_min + ((float)i / (float)num_segments) * IM_PI * 2.0f;
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a) * radius,
				centre.y + ImSin(a) * radius));
		}
		window->DrawList->PathStroke(color, false, thickness);
		return centre;
	}
}

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
	picture_dir = std::string("./UI/Pictures");
	my_window = window;
	GUI_show_buy_ui = false;
	GUI_show_minimap = true;
	GUI_show_stamina = true; 
	GUI_show_scoreboard = true; 
	GUI_show_sale_ui = false; 
	GUI_show_timer = true; 
	prev_show_sale_ui = false; 
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

static std::unordered_map<int, InputCommands> buy_command_map = {
	{1, BUY_CARROT},
	{2, BUY_CABBAGE},
	{3, BUY_CORN},
	{4, BUY_RADISH},
	{5, BUY_TOMATO},
};

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

	/* build scoreboard */
	if (GUI_show_scoreboard) {
		ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
		ImGui::Begin("ScoreBoard_bg", NULL, TRANS_WINDOW_FLAG);
		ImGui::Image((void*)(intptr_t)score_background.my_image_texture, ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
		ImGui::End();
		ImGui::SetNextWindowPos(ImVec2(190 * display_ratio, 190 * display_ratio), 0, ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
		ImGui::Begin("ScoreBoard_content", NULL, TRANS_WINDOW_FLAG);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(51, 48, 49, 255));

		for (int i = 0; i < NUM_ICON; i++) {
			GUIImage image = icon_images_list[i];
			ImGui::Image((void*)(intptr_t)image.my_image_texture, ImVec2(image.my_image_width * display_ratio, image.my_image_height * display_ratio));
			ImGui::SameLine();
			ImGui::Text("%d", scoreboard_data[i]);
		}
		ImGui::PopStyleColor();
		ImGui::End();
	}
	/* end of scoreboard */

	/* build the sale page */
	if(GUI_show_sale_ui) {		
		//TODO: now it can only trigger the sale page, need to use another boolean if want to trigger sale and buy page seperately
		if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
			if (rack_image_idx < NUM_RACK_IMG - 1)
				rack_image_idx++;
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
			if (rack_image_idx > 1)
				rack_image_idx--;
		}
		else if(ImGui::IsKeyPressed(ImGuiKey_Enter)) {
			InputManager::lastCmd = buy_command_map[rack_image_idx];
		}
		// etc.
		bool open_ptr = true;
		GUIImage* rack_image = &rack_images_list[rack_image_idx];
		GUIImage fish_image = fish_images_list[rack_image_idx%3];

		ImVec2 rack_size = ImVec2(rack_image->my_image_width * display_ratio, rack_image->my_image_height * display_ratio);
		ImVec2 fish_size = ImVec2(window_width, window_width * fish_image.my_image_height / fish_image.my_image_width);
		ImGui::SetNextWindowSize(ImVec2(window_width,window_height));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("Sale GUI", &open_ptr, TRANS_WINDOW_FLAG);
		
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image((void*)(intptr_t)fish_image.my_image_texture, fish_size);

		ImGui::SetCursorPos(ImVec2(window_width - rack_size.x, window_height * 0.9 - rack_size.y ));
		if ( rack_image_idx == 1 && rack_image->fade_ratio > 0.001) {
			ImGui::SetCursorPos(ImVec2(window_width - rack_size.x + rack_image->fade_ratio * rack_size.x * 2 , window_height * 0.9 - rack_size.y));
			if (rack_image->fade_in) {
				rack_image->fade_ratio = rack_image->fade_ratio < 0.001 ? 0.001 : rack_image->fade_ratio * 0.8;
			}
		}
		ImGui::Image((void*)(intptr_t)rack_image->my_image_texture, rack_size);
		ImGui::End();
	} else {
		GUIImage* rack_image = &rack_images_list[1];
		rack_image->fade_ratio = 1; 
	}

	prev_show_sale_ui = GUI_show_sale_ui;
	

	/*end of sale page*/
	if (GUI_show_minimap) {
		createMiniMap();
	}
	if (GUI_show_stamina) {
		stamina_percent = 0.85; //TODO hardcode stamina percentage
		createStamina();
	}
	if (GUI_show_timer) {
		float padding = 64.0f * display_ratio;
		int width = 600;
		int height = 600;
		ImVec2 size = ImVec2(width * display_ratio, height * display_ratio);
		const ImU32 col = IM_COL32(245.f, 61.f, 119.f, 200);//ImGui::GetColorU32(ImGuiCol_ButtonHovered);
		//float ratio = GUI_timer_percent;
		float ratio = timer_percent;
		ImGui::SetNextWindowSize(size);
		ImGui::SetNextWindowPos(ImVec2(window_width - padding - size.x, padding + size.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
		ImGui::Begin("Timer", NULL, TRANS_WINDOW_FLAG);
		ImVec2 center = ImGui::Spinner("##spinner", 200 * display_ratio, 80 * display_ratio, ratio, col);
		ImGui::End();
		ImGui::SetNextWindowPos(ImVec2(window_width - padding - size.x, padding + size.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
		ImGui::SetNextWindowSize(size);
		ImGui::Begin("Timer text", NULL, TRANS_WINDOW_FLAG);
		float font_size = ImGui::GetFontSize();
		float text_size = font_size * GUI_timer_string.size() / 2;
		ImGui::SetCursorPos(ImVec2(size.x * 0.5f - text_size * 0.5f, size.y * 0.5f - font_size * 0.5f));
		ImGui::Text(GUI_timer_string.c_str());
		ImGui::End();
	}

	//test fading out
	/*float padding = 64.0f;
	int test_width = 2000; 
	int test_height = 2000; 
	ImVec2 test_size = ImVec2(test_width * display_ratio, test_height*display_ratio);
	ImGui::SetNextWindowSize(test_size);
	ImGui::SetNextWindowPos(ImVec2(window_width - padding - test_size.x, padding ));
	bool bptr; 
	ImGui::Begin("testing fade" , & bptr, TRANS_WINDOW_FLAG );
	GUIImage* image = &rack_images_list[0];
	ImVec2 image_size = ImVec2(image->my_image_width * display_ratio * 0.5f, image->my_image_height * display_ratio * 0.5f);
	ImGui::SetCursorPos(ImVec2(image_size.x* image->fade_ratio , 0 ));
	ImGui::Image((void*)(intptr_t)image->my_image_texture, image_size);
	if (image->fade_in) {
		image->fade_ratio *= 0.8;
		image->fade_ratio = image->fade_ratio < 0.001 ? 0.001: image->fade_ratio;
	}else if (image->fade_ratio < 1) {
		image->fade_ratio *= 1.25; 
	}
	ImGui::End(); 
	if (ImGui::IsKeyPressed(ImGuiKey_R)) {
		image->fade_in = !image->fade_in; 
	}*/
	//end of fading out test
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	return GUI_show_sale_ui;
}

// Cleanup IMGUI, should be called after the mainloop
void GUI::cleanUp(){
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
		image->my_image_width *= 0.9f;
		image->my_image_height *= 0.9f;
		i++;
	}
	rack_image_idx = 1;
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

	LoadTextureFromFile((picture_dir + std::string("/minimap_background.png")).c_str(), &(minimap_background.my_image_texture),
		&(minimap_background.my_image_width), &(minimap_background.my_image_height));
	i = 0; 
	const char* fish_dir = (picture_dir + std::string("/fishy")).c_str();
	for (auto& entry : fs::directory_iterator(fish_dir)) {
		//std::cout << entry.path() << std::endl;
		GUIImage* image = &(fish_images_list[i]);
		const char* epath = entry.path().string().c_str();
		bool ret = LoadTextureFromFile(epath, &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
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
	if (GUI_show_sale_ui!=show) {
		rack_image_idx = 1;
	}
	GUI_show_sale_ui = show;
	return show;
}
/**
	this method construct the minimap on the corner of the screen base on the player locations
	only add the require elements for minimap but not actually render it
*/
void GUI::createMiniMap() {
	//get size of minimap 
	float world_dim = 500.0f;
	
	float padding = 12.0f * display_ratio;
	int width = minimap_background.my_image_width * display_ratio;
	int height = minimap_background.my_image_height * display_ratio;
	bool bptr;

	//size of minimap 
	ImVec2 image_size = ImVec2(width, height);
	ImVec2 window_size = ImVec2(width + padding, height + padding); 

	ImGui::SetNextWindowSize(window_size);
	ImGui::SetNextWindowPos(ImVec2(padding, window_height - padding), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::Begin("Minimap background", &bptr, TRANS_WINDOW_FLAG);
	ImGui::Image((void*)(intptr_t)minimap_background.my_image_texture, image_size);
	ImGui::End(); 

	ImGui::SetNextWindowSize(window_size);
	ImGui::SetNextWindowPos(ImVec2(padding, window_height - padding), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	//ImVec2 center = ImVec2(padding + size.x / 2, window_height - padding - size.y / 2); // center of minimap
	ImVec2 center = ImVec2(image_size.x / 2.0f, image_size.y /2.0f); // center of minimap

	ImGui::Begin("MiniMap", &bptr, TRANS_WINDOW_FLAG);
	//place all player's icon:
	for (int i = 0; i < 4; i++) {
		GUIImage image = icon_images_list[i];
		//TODO convert the pos into relative pos in minimap
		ImVec2 icon_size = ImVec2(image.my_image_width * display_ratio, image.my_image_height * display_ratio);

		ImVec2 relative_pos = ImVec2(player_pos[i].x / world_dim * image_size.x- icon_size.x * 0.5f, player_pos[i].y / world_dim * image_size.y - icon_size.y * 0.5f); 
		//size of icon 
		ImGui::SetCursorPos(relative_pos + center);
		ImGui::Image((void*)(intptr_t)image.my_image_texture, icon_size);
	}
	ImGui::End();
}

void GUI::createStamina() {
	bool bptr;
	int width = 160;
	int height = 1600; 
	float padding = 64.0f * display_ratio;
	const ImU32 col = IM_COL32(245.f, 61.f, 119.f, 80);//ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	const ImU32 bg = IM_COL32(227.f, 188.f, 208.f, 200); //ImGui::GetColorU32(ImGuiCol_Button);

	ImVec2 size = ImVec2(width * display_ratio, height * display_ratio);

	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(ImVec2(window_width - padding - size.x, window_height - padding), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::Begin("Stamina Bar", &bptr, TRANS_WINDOW_FLAG);
	ImGui::ReverseBufferingBar("##Stamina_bar", stamina_percent, size, bg, col);
	ImGui::End();

}

void GUI::createTimer(float ratio) {
	float padding = 64.0f * display_ratio;
	int width = 600;
	int height = 600;
	ImVec2 size = ImVec2(width * display_ratio, height * display_ratio);
	const ImU32 col = IM_COL32(245.f, 61.f, 119.f, 200);//ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	//float ratio = GUI_timer_percent;
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(ImVec2(window_width - padding - size.x, padding + size.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::Begin("Timer", NULL,TRANS_WINDOW_FLAG);
	ImVec2 center = ImGui::Spinner("##spinner", 200*display_ratio, 80*display_ratio, ratio, col);
	ImGui::End(); 
	ImGui::SetNextWindowPos(ImVec2(window_width - padding - size.x, padding + size.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::SetNextWindowSize(size);
	ImGui::Begin("Timer text", NULL, TRANS_WINDOW_FLAG);
	float font_size = ImGui::GetFontSize();
	float text_size = font_size * GUI_timer_string.size() / 2; 
	ImGui::SetCursorPos(ImVec2(size.x*0.5f - text_size * 0.5f, size.y*0.5f - font_size * 0.5f));
	ImGui::Text(GUI_timer_string.c_str()); 
	ImGui::End();
}

void GUI::renderWaitingClient(int client_joined, int max_client) {
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

	ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Loading Background", NULL, TRANS_WINDOW_FLAG);
	ImGui::Image((void*)(intptr_t)loading_background.my_image_texture, \
		ImVec2(window_width, window_width * loading_background.my_image_height / loading_background.my_image_width ));
	ImGui::End();
	ImGui::SetNextWindowPos(ImVec2(32.0f * display_ratio, window_height / 3 * 2));
	ImGui::Begin("Loading Status", NULL, TRANS_WINDOW_FLAG);
	ImGui::Text("%d of %d client joined...");
	ImGui::End(); 

	// Rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(my_window);
}

void GUI::setTimer(float time) {
	timer_percent = time; 
}





