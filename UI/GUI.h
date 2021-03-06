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
#include <unordered_set>
#include <algorithm>

#define NUM_ICON 4 // should be consistent with the number of character, will be used in multiple place
#define NUM_RACK_IMG 6
#define NUM_LOAD_IMG 8
#define NUM_CHASE_IMG 2 
#define NUM_FISH_IMG 3
#define NUM_TOOL_IMG 10
#define NUM_VEG_IMG 6
#define NUM_RES_IMG 4
#define SIGN_TIME_INTERVAL 5
#define TRANS_WINDOW_FLAG ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar| \
						  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | \
						  ImGuiWindowFlags_NoInputs 

#define DRAK_WINDOW_FLAG ImGuiWindowFlags_NoTitleBar| \
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
	static bool show_eggplant_sign;
	static bool show_eggplant_sign_prev; // this bool is used to record if the sign is already trigger before
	static bool show_glued_sign;
	static bool show_soju_sign;
	static int rack_image_idx;
	static int tool_image_idx;
	static int char_selection_idx;
	static int veggie_sale_idx; 
	static float timer_percent;
	static float remaining_sec;
	static float stamina_percent; //use this variable to set the stamina display
	static int eggplant_spawn_time; 
	static int my_player_idx; 
	static int soju_sign_time;
	static int glue_sign_time;

	static float winning_fade_ratio;
	static bool GUI_show_winning;
	static std::string GUI_timer_string;

	static ImFont* font_Are_You_Serious;
	static ImFont* font_Fredericka_the_Great;
	static ImFont* font_Mystery_Quest;
	static ImFont* font_Ranchers;
	static ImFont* font_Ranchers_large;
	static ImFont* font_Sofia_Not_Nicks_Gf;
	static ImFont* font_Sofia_Nicks_Gf;

	static GUIImage rack_images_list[NUM_RACK_IMG];
	static std::unordered_map<ModelEnum, GUIImage> icon_images_map;
	static GUIImage char_images_list[NUM_ICON];
	static GUIImage chase_images_list[NUM_CHASE_IMG];
	static GUIImage fish_images_list[NUM_FISH_IMG];
	static GUIImage tool_images_list[NUM_TOOL_IMG];
	static GUIImage veg_images_list[NUM_VEG_IMG];
	static GUIImage res_images_list[NUM_RES_IMG];
	static GUIImage sale_background;
	static GUIImage curtain_img;
	static GUIImage stamina_image;
	static GUIImage timer_background;
	static GUIImage eggplant_sign_img; 
	static GUIImage two_min_sign_img;
	static GUIImage you_win_img; 
	static GUIImage soju_sign_img;
	static GUIImage glue_sign_img; 

	static int scoreboard_data[NUM_ICON];
	static std::string characters_name_list[];
	static char* seed_type_list[];
	static char* tool_type_list[];
	static char* tool_func_list[];
	static float tool_sell_list[];
	static char* seed_prop_list[];
	static float veg_price_list[];
	static float veg_sell_list[];
	static float veg_time_list[];
	static char* veg_type_list[];
	

	static ImVec2 player_pos[4];
	static std::vector<ModelEnum> char_selections;

	static void initializeGUI(GLFWwindow* window);
	static bool renderUI();
	static void cleanUp();
	static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
	static void initializeImage();
	static bool renderLoadScene(GLFWwindow* window);
	static void initializeLoadingImage();
	static bool renderProgressBar(float percent, GLFWwindow* window, bool flip_image);
	static void renderWaitingClient(int client_joined, int max_client);
	static ModelEnum renderCharacterSelection(std::unordered_map<ModelEnum, int>& selected_chars, int client_idx);
	static void createMiniMap();
	static void updateDisplayRatio(int width, int height);
	static bool ShowGUI(bool show);
	static void createStamina();
	static void createTimer(float ratio);
	static void setTimer(float time, float remaining_sec);
	static bool renderWinningScene();
	static void createSaleConfirmation();
	static void setHoldingModel(ModelEnum model);
	static void setShowSaleUI(bool show);
	static void createTopSign(bool* show, GUIImage* image);
	static void createBottomSign(std::string label, bool* show, GUIImage* image, int time);

	static void setShowGoldenEggplantSign(bool show); 
	static void setShowGlueSign(bool show);
	static void setShowSojuSign(bool show);



};