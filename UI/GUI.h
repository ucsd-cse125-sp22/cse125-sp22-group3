#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
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
#define TRANS_WINDOW_FLAG ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar| \
						  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar

#define MINI_MAP_FLAG ImGuiWindowFlags_NoTitleBar| \
						  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar

//using namespace std;

struct GUIImage {
public: 
	int my_image_width = 0;
	int my_image_height = 0;
	GLuint my_image_texture = 0;

};
class GUI {
public:
	static bool show_GUI;
	static float display_ratio; 
	static int window_height;
	static int window_width; 
	static std::string picture_dir;
	static GLFWwindow* my_window;
	static GUIImage score_background;
	static GUIImage loading_background; 
	static GUIImage loading_bg[NUM_LOAD_IMG]; 
	static bool show_loading;
	static int rack_image_idx;
	static GUIImage rack_images_list[NUM_RACK_IMG];
	static GUIImage icon_images_list[NUM_ICON];
	static GUIImage chase_images_list[NUM_CHASE_IMG];
	static int scoreboard_data[NUM_ICON];
	static ImVec2 player_pos[4]; 
	static void initializeGUI(GLFWwindow* window);
	static bool renderUI();
	static void cleanUp();
	static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
	static void initializeImage();
	static bool renderLoadScene(GLFWwindow* window);
	static void initializeLoadingImage(); 
	static bool renderProgressBar(float percent, GLFWwindow* window, bool flip_image);
	static void createMiniMap();
	static void updateDisplayRatio(int width, int height);
	static bool ShowGUI(bool show);
};


