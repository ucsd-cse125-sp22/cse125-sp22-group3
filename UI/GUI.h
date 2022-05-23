#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../InputManager.h"
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <vector>
#include "../stb_image.h"
#include <string>
#define IMGUI_DEFINE_MATH_OPERATORS //include to use imVec2 math operation 
#include "imgui/imgui_internal.h"
#include <thread>         
#include <chrono>

#define NUM_ICON 4
#define NUM_RACK_IMG 6
#define NUM_LOAD_IMG 8
#define NUM_CHASE_IMG 2 
#define NUM_FISH_IMG 3
#define NUM_TOOL_IMG 10
#define TRANS_WINDOW_FLAG ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar| \
						  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | \
						  ImGuiWindowFlags_NoInputs 

#define MINI_MAP_FLAG ImGuiWindowFlags_NoTitleBar| \
						  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar


//using namespace std;

struct GUIImage {
public: 
	int my_image_width = 0;
	int my_image_height = 0;
	GLuint my_image_texture = 0;
	float fade_ratio = 0.001; 
	bool fade_in = true; // true for fade in, false for fade out; 

};
class GUI {
public:
	static float display_ratio; 
	static int window_height;
	static int window_width; 
	static std::string picture_dir;
	static GLFWwindow* my_window;
	static GUIImage score_background;
	static GUIImage loading_background; 
	static GUIImage minimap_background; 
	static GUIImage loading_bg[NUM_LOAD_IMG]; 

	static bool sale_tools; // default to false ( saleing seed), true if show tool shed
	static bool show_loading;
	static int rack_image_idx;
	static int tool_image_idx; 
	static float timer_percent; 
	static float stamina_percent; //use this variable to set the stamina display

	static float winning_fade_ratio; 
	static bool GUI_show_winning; 
	static std::string GUI_timer_string;
	
	static ImFont* font_Are_You_Serious;
	static ImFont* font_Fredericka_the_Great;
	static ImFont* font_Mystery_Quest;
	static ImFont* font_Ranchers; 

	static GUIImage rack_images_list[NUM_RACK_IMG];
	static GUIImage icon_images_list[NUM_ICON];
	static GUIImage chase_images_list[NUM_CHASE_IMG];
	static GUIImage fish_images_list[NUM_FISH_IMG];
	static GUIImage tool_images_list[NUM_TOOL_IMG];
	static GUIImage curtain_img;
	static GUIImage stamina_image; 
	static GUIImage timer_background; 
	static int scoreboard_data[NUM_ICON];
	static char* seed_type_list[];
	static char* tool_type_list[];
	static char* tool_func_list[];

	static ImVec2 player_pos[4]; 


	static void initializeGUI(GLFWwindow* window);
	static bool renderUI();
	static void cleanUp();
	static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
	static void initializeImage();
	static bool renderLoadScene(GLFWwindow* window);
	static void initializeLoadingImage(); 
	static bool renderProgressBar(float percent, GLFWwindow* window, bool flip_image);
	static void renderWaitingClient(int client_joined, int max_client); 
	static void createMiniMap();
	static void updateDisplayRatio(int width, int height);
	static bool ShowGUI(bool show);
	static void createStamina();
	static void createTimer(float ratio); 
	static void setTimer(float time);
	static bool renderWinningScene();
};