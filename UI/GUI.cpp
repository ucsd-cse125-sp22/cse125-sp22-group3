#include "GUI.h"
#include "../util.h"
#include <iostream>
//#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;
#include <filesystem>
namespace fs = std::filesystem;

GUIImage GUI::rack_images_list[NUM_RACK_IMG];
std::unordered_map<ModelEnum, GUIImage> GUI::icon_images_map;
std::vector<ModelEnum> GUI::char_selections;
GUIImage GUI::score_background; 
GUIImage GUI::loading_background;
GUIImage GUI::minimap_background;


GUIImage GUI::loading_bg[NUM_LOAD_IMG];
GUIImage GUI::chase_images_list[NUM_CHASE_IMG];
int GUI::scoreboard_data[NUM_ICON];
GUIImage GUI::fish_images_list[NUM_FISH_IMG];
GUIImage GUI::tool_images_list[NUM_TOOL_IMG];
GUIImage GUI::veg_images_list[NUM_VEG_IMG];
GUIImage GUI::char_images_list[NUM_ICON];
int GUI::char_selection_idx;
int GUI::remaining_sec;
int GUI::veggie_sale_idx; 


GUIImage GUI::curtain_img;
GUIImage GUI::stamina_image;
GUIImage GUI::timer_background;
GUIImage GUI::sale_background;

float GUI::display_ratio;
int GUI::window_height;
int GUI::window_width;
float GUI::timer_percent;
float GUI::stamina_percent;
std::string GUI::GUI_timer_string;
bool GUI::sale_tools; // default to false ( saleing seed), true if show tool shed

int GUI::rack_image_idx; 
int GUI::tool_image_idx;

std::string GUI::picture_dir;
GLFWwindow* GUI::my_window;
ImVec2 GUI::player_pos[4];


ImFont* GUI::font_Are_You_Serious;
ImFont* GUI::font_Fredericka_the_Great;
ImFont* GUI::font_Mystery_Quest;
ImFont* GUI::font_Ranchers;

float GUI::winning_fade_ratio = 1;
bool GUI::GUI_show_winning;

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

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		//window->DrawList->AddRectFilled(bb.Min, bb.Max, bg_col, size.x * 0.4f, ImDrawFlags_RoundCornersTop);
		window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y + size.y * (1 - value)), bb.Max, fg_col, size.x * 0.4f, ImDrawFlags_RoundCornersTop);
		return true;
	}
	ImVec2 Spinner(const char* label, float radius, int thickness, float ratio, const ImU32& color, int num_segments) {
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

		int start = 1; //abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

		const float a_min = IM_PI * 1.5f; // IM_PI * 2.0f * ((float)start) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius + thickness, pos.y + radius + thickness);

		for (int i = 0; i <= (num_segments+1) * ratio; i++) {
			const float a = a_min + ((float)i / (float)num_segments) * IM_PI * 2.0f;
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a) * radius,
				centre.y + ImSin(a) * radius));
		}
		window->DrawList->PathStroke(color, false, thickness);
		return centre;
	}

	bool BlackBanner(const char* label, int window_width, int window_height, float ratio, bool top = true, float width_ratio = 0.125f) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImVec2 size = ImVec2(window_width, window_height);
		ImVec2 rect_size = ImVec2(window_width, window_height * width_ratio );
		ImVec2 pos = window->DC.CursorPos;
		const ImU32 black = IM_COL32(0, 0, 0, 255);

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImRect bb(pos, pos + rect_size);
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;
		if (top) {
			window->DrawList->AddRectFilled(bb.Min, ImVec2(bb.Max.x, bb.Max.y * ratio), black);
		} else {
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y + rect_size.y * (1-ratio)),bb.Max, black);

		}
		return true;
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
	font_Are_You_Serious = io.Fonts->AddFontFromFileTTF("./UI/fonts/Are_You_Serious/AreYouSerious-Regular.ttf", 36.0f);
	font_Fredericka_the_Great = io.Fonts->AddFontFromFileTTF("./UI/fonts/Fredericka_the_Great/FrederickatheGreat-Regular.ttf", 36.0f);
	font_Mystery_Quest = io.Fonts->AddFontFromFileTTF("./UI/fonts/Mystery_Quest/MysteryQuest-Regular.ttf", 36.0f);
	font_Ranchers = io.Fonts->AddFontFromFileTTF("./UI/fonts/Ranchers/Ranchers-Regular.ttf", 36.0f);

	picture_dir = std::string("./UI/Pictures");
	my_window = window;
	GUI_show_buy_ui = false;
	GUI_show_minimap = true;
	GUI_show_stamina = true; 
	GUI_show_scoreboard = true; 
	GUI_show_buy_ui= false; 
	GUI_show_timer = true; 
	GUI_show_winning = false; 
	stamina_percent = 100; 
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
	if (width > 0 && height > 0) {
		display_ratio = 0.2f * width / 1280;
		window_width = width;
		window_height = height;
	}
}

static std::unordered_map<int, InputCommands> buy_command_map = {
	{1, BUY_CARROT},
	{2, BUY_CABBAGE},
	{3, BUY_CORN},
	{4, BUY_RADISH},
	{5, BUY_TOMATO},
	{6, BUY_NET},
	{7, BUY_HOE},
	{8, BUY_WATER},
	{9, BUY_FERTILIZER},
	{10, BUY_SHOVEL},
	{11, BUY_GLUE},
	{12, BUY_POISON},
	{13, BUY_OATS},
	{14, BUY_SOJU},
};

char* GUI::seed_type_list[] = { "Carrot", "Cabbage", "Corn","Radish", "Tomato"};
char* GUI::tool_type_list[] = { "Net", "Hoe", "Watering Can","Fertilizer", "Shovel", "Glue","Poison", "Super Oats", "Farmer's Ale"};
char* GUI::tool_func_list[] = {
	"steal a veggie that is planted",
	"create a new plot for yourself",
	"water certain veggies",
	"fertilize certain veggies",
	"uproot someone's plot",
	"slow people down",
	"poison someone's vegetable",
	"give you a stamina boost",
	"get other player drunk"
};
float GUI::tool_sell_list[] = { tool_map[ModelEnum::NET].price,
tool_map[ModelEnum::HOE].price,
tool_map[ModelEnum::WATERING_CAN].price,
tool_map[ModelEnum::FERTILIZER].price,
tool_map[ModelEnum::SHOVEL].price,
tool_map[ModelEnum::GLUE].price,
tool_map[ModelEnum::POISON].price,
tool_map[ModelEnum::OATS].price,
tool_map[ModelEnum::SOJU].price,
};

char* GUI::seed_prop_list[] = {
	"won't require much care",
	"will require some water",
	"won't require much care",
	"will require some fertilizer",
	"will require water and fertilizer"
};
float GUI::veg_price_list[] = { veggie_map[VegetableType::CARROT].seed_price,
veggie_map[VegetableType::CABBAGE].seed_price,
veggie_map[VegetableType::CORN].seed_price,
veggie_map[VegetableType::RADISH].seed_price,
veggie_map[VegetableType::TOMATO].seed_price};

float GUI::veg_sell_list[] = { veggie_map[VegetableType::CARROT].sell_price,
veggie_map[VegetableType::CABBAGE].sell_price,
veggie_map[VegetableType::CORN].sell_price,
veggie_map[VegetableType::RADISH].sell_price,
veggie_map[VegetableType::TOMATO].sell_price };

float GUI::veg_time_list[] = { veggie_map[VegetableType::CARROT].growth_time,
veggie_map[VegetableType::CABBAGE].growth_time,
veggie_map[VegetableType::CORN].growth_time,
veggie_map[VegetableType::RADISH].growth_time,
veggie_map[VegetableType::TOMATO].growth_time };

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
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	/* build scoreboard */
	if (GUI_show_scoreboard) {
		ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
		ImGui::Begin("ScoreBoard_bg", NULL, TRANS_WINDOW_FLAG);
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image((void*)(intptr_t)score_background.my_image_texture, ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
		ImGui::End();
		ImGui::SetNextWindowPos(ImVec2(190 * display_ratio, 190 * display_ratio), 0, ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(score_background.my_image_width * display_ratio, score_background.my_image_height * display_ratio));
		ImGui::Begin("ScoreBoard_content", NULL, TRANS_WINDOW_FLAG);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(51, 48, 49, 255));

		for (int i = 0; i < char_selections.size(); i++) {
			GUIImage image = icon_images_map[char_selections[i]];
			ImGui::Image((void*)(intptr_t)image.my_image_texture, ImVec2(image.my_image_width * display_ratio, image.my_image_height * display_ratio));
			ImGui::SameLine();
			ImGui::Text("%d", scoreboard_data[i]);
		}
		ImGui::PopStyleColor();
		ImGui::End();
	}
	/* end of scoreboard */

	/* build the sale page */
	if(GUI_show_buy_ui) {
		
		//TODO: now it can only trigger the sale page, need to use another boolean if want to trigger sale and buy page seperately
		// press up arrow key to return to the seed rack
		if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
			if (!sale_tools && rack_image_idx-3>0) {
				rack_image_idx -=3;
			}
			else if (sale_tools) {
				switch (tool_image_idx) {
					case 1:
						tool_image_idx = 8;
						break;
					case 2:
						tool_image_idx = 8;
						break;
					case 3:
						tool_image_idx = 6;
						break;
					case 4:
						tool_image_idx = 7;
						break;
					case 5:
						tool_image_idx = 9;
						break;
					case 6:
						tool_image_idx = 8;
						break;
					case 7:
						tool_image_idx = 9;
						break;
					default:
						break;

				}
			
			}
		
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
			if (!sale_tools && rack_image_idx + 3 <NUM_RACK_IMG) {
				rack_image_idx += 3;
			}
			else if (sale_tools) {
				switch (tool_image_idx) {
				case 6:
					tool_image_idx = 3;
					break;
				case 7:
					tool_image_idx = 4;
					break;
				case 8:
					tool_image_idx = 6;
					break;
				case 9:
					tool_image_idx = 7;
					break;
				default:
					break;

				}
			}
		}

		else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
			if (sale_tools && tool_image_idx == 5) {
				rack_image_idx = 1;
				curtain_img.fade_in = true;

				(&rack_images_list[1])->fade_in = true;
				sale_tools = false;
			}
			else if (!sale_tools && rack_image_idx == NUM_RACK_IMG - 1) {
				tool_image_idx = 1;
				curtain_img.fade_in = false;
				(&rack_images_list[1])->fade_in = false;
				sale_tools = true;
			}
			else if (!sale_tools && rack_image_idx < NUM_RACK_IMG - 1) {
				rack_image_idx++;
			}
			else if (sale_tools) {// && tool_image_idx < NUM_TOOL_IMG - 1) { -> incrementing highest index (9) should go to idx 5
				switch (tool_image_idx) {
				case 7:
					tool_image_idx = 5;
					break;
				case 9:
					tool_image_idx = 5;
					break;
				default:
					tool_image_idx++;
					break;
				}

			}
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
			if (sale_tools && tool_image_idx == 1) {
				rack_image_idx = NUM_RACK_IMG - 1;
				curtain_img.fade_in = true;
				(&rack_images_list[1])->fade_in = true;
				sale_tools = false;
			}
			else if (!sale_tools && rack_image_idx == 1) {
				tool_image_idx = 5;
				curtain_img.fade_in = false;
				(&rack_images_list[1])->fade_in = false;
				sale_tools = true;
			}
			else if (!sale_tools && rack_image_idx > 1) {
				rack_image_idx--;
			}
			else if (sale_tools && tool_image_idx > 1) {
				switch (tool_image_idx) {
				case 6:
					tool_image_idx = 2;
					break;
				case 8:
					tool_image_idx = 2;
					break;
				default:
					tool_image_idx--;
					break;
				}
			}
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
			// TODO: send buy command for buying tools
			
			if(sale_tools)
				InputManager::lastCmd = buy_command_map[5+tool_image_idx];
			else
				InputManager::lastCmd = buy_command_map[rack_image_idx];
			sale_tools = false;
		}


		// etc.
		bool open_ptr = true;
		GUIImage* rack_image_actual = &rack_images_list[rack_image_idx];
		GUIImage* rack_image = &rack_images_list[1]; 
		GUIImage* tool_image = &tool_images_list[tool_image_idx]; 
		GUIImage fish_image = fish_images_list[(rack_image_idx+tool_image_idx)%3];

		ImVec2 fish_size = (1.0f *window_width) / window_height > (1.0f *fish_image.my_image_width) / fish_image.my_image_height ?
						   ImVec2(window_height * fish_image.my_image_width / fish_image.my_image_height, window_height):
						   ImVec2(window_width, window_width * fish_image.my_image_height / fish_image.my_image_width);
		ImVec2 rack_size = ImVec2(rack_image->my_image_width * (fish_size.y * 0.5f)/ rack_image->my_image_height  , fish_size.y * 0.5f);

		ImVec2 tool_size = fish_size;
		ImVec2 curtain_size = ImVec2(fish_size.y/curtain_img.my_image_height * curtain_img.my_image_width, fish_size.y);
		ImVec2 window_size = ImVec2(window_width, window_height);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 255));
		ImGui::SetNextWindowSize(window_size);
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("Buy_UI_bg",&open_ptr, DRAK_WINDOW_FLAG);
		ImGui::End(); 
		ImGui::SetNextWindowSize(fish_size);
		ImGui::SetNextWindowPos((window_size-fish_size)*0.5f);
		ImGui::Begin("Buy Store GUI", &open_ptr, DRAK_WINDOW_FLAG);
		ImGui::SetScrollX(0); //prevent scroll cause the context is actually larger than the window 
		ImGui::SetScrollY(0); 
		// add tool shed layer
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image((void*)(intptr_t)tool_image->my_image_texture, tool_size); 
		
		// add the curtain layer 
		ImGui::SetCursorPos(ImVec2(window_width - curtain_size.x + curtain_img.fade_ratio * curtain_size.x, 0));
		if (curtain_img.fade_in) {
			curtain_img.fade_ratio = curtain_img.fade_ratio < 0.001 ? 0.001 : curtain_img.fade_ratio * 0.8;
		} else {
			curtain_img.fade_ratio = curtain_img.fade_ratio < 1 ? curtain_img.fade_ratio * 1.5 : 1; 
		}
		ImGui::Image((void*)(intptr_t)curtain_img.my_image_texture, curtain_size);

		// add the fish layer
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image((void*)(intptr_t)fish_image.my_image_texture, fish_size);

		// add the rack layer
		ImGui::SetCursorPos(ImVec2(fish_size.x - rack_size.x + rack_image->fade_ratio * rack_size.x, fish_size.y * 0.9 - rack_size.y));
		if (rack_image->fade_in) {
			rack_image->fade_ratio = rack_image->fade_ratio <= 0.001 ? 0.001 : rack_image->fade_ratio * 0.8;
		} else {
			rack_image->fade_ratio = rack_image->fade_ratio < 1?  rack_image->fade_ratio*1.5 : 1;
		}
		ImGui::Image((void*)(intptr_t)rack_image_actual->my_image_texture, rack_size);

		ImGui::PushFont(font_Are_You_Serious);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(51, 48, 49, 255));

		//show talking box
		if (sale_tools) {
			int currIdx = tool_image_idx - 1;
			ImGui::SetCursorPos(ImVec2(fish_size.x*0.125, fish_size.x * 0.125));
			ImGui::Text("Oh my! The %s!\n It can be used to %s.\nThat will be %.0f dollar(s).\nPress [Enter] to buy!", \
				tool_type_list[currIdx], tool_func_list[currIdx],tool_sell_list[currIdx]);
		} else {
			int currIdx = rack_image_idx - 1;
			ImGui::SetCursorPos(ImVec2(fish_size.x * 0.125, fish_size.x * 0.125));
			ImGui::Text("Why yes! A %s seed!\nThe %s requires %.0f seconds to grow\n    and %s.\nAfter harvest, it will sell for %.0f dollars(s).\nThat will be %.0f dollar(s).\nPress [Enter] to buy!",
				seed_type_list[currIdx], seed_type_list[currIdx], veg_time_list[currIdx], seed_prop_list[currIdx],veg_sell_list[currIdx],veg_price_list[currIdx]);
		}
		ImGui::PopFont(); 
		ImGui::PopStyleColor(2);

		ImGui::End();
	} 

	/*end of sale page*/
	if (GUI_show_minimap) {
		createMiniMap();
	}
	if (GUI_show_stamina) {
		//stamina_percent = 0.85; //TODO hardcode stamina percentage
		createStamina();
	}
	if (GUI_show_timer) {
		int radius = 150 * display_ratio;
		int thickness = 60 * display_ratio;
		ImVec2 spinner_size = ImVec2((radius + thickness)*2, (radius + thickness)*2); 
		//const ImU32 col = IM_COL32(245.f, 61.f, 119.f, 255);//ImGui::GetColorU32(ImGuiCol_ButtonHovered);
		//const ImU32 bg = IM_COL32(227.f, 188.f, 208.f, 255); //ImGui::GetColorU32(ImGuiCol_Button);
		
		float ratio = timer_percent; 
		//float ratio = GUI_timer_percent;
		//std::cout << "the timer percent is " << ratio << std::endl;
		ImVec2 bg_size = ImVec2(timer_background.my_image_width * display_ratio, timer_background.my_image_height * display_ratio);

		ImVec2 padding = ImVec2(bg_size.x * 0.5f, bg_size.y * 0.25f);
		
		ImGui::SetNextWindowPos(ImVec2(window_width - bg_size.x, bg_size.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
		ImGui::SetNextWindowSize(bg_size);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::Begin("Timer_bg", NULL, TRANS_WINDOW_FLAG);
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image((void*)(intptr_t)timer_background.my_image_texture, bg_size);
		const ImU32 bg = IM_COL32(219, 195, 182, 255); //ImGui::GetColorU32(ImGuiCol_Button);
		ImU32 col = IM_COL32(148, 102, 77, 255);
		if (remaining_sec <= 120) {
			col = IM_COL32(255, 71, 61, 255);
		}

		ImGui::SetCursorPos(padding);
		ImGui::Spinner("##spinner", radius, thickness, 1, col, 30);
		ImGui::SetCursorPos(padding);
		ImGui::Spinner("##spinner", radius, thickness, ratio, bg, 120);
		if (remaining_sec > 120 || remaining_sec % 2 == 0) {
			auto text_size = ImGui::CalcTextSize(GUI_timer_string.c_str());
			ImGui::SetCursorPos((spinner_size - text_size) * 0.5f + padding);
			ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::Text(GUI_timer_string.c_str());
			ImGui::PopStyleColor();
		}
		ImGui::End();
		//ImGui::PopStyleVar(2);
	}

	//TODO: for testing purpose, remove after connect to server
	if (ImGui::IsKeyPressed(ImGuiKey_B)) {
		GUI_show_sale = !GUI_show_sale; 
	}
	if (GUI_show_sale) {
		createBuyConfirmation();
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
	ImGui::PopStyleVar(2);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	return GUI_show_buy_ui;
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
	int i = 0;
	const std::string rack_path = picture_dir + std::string("/rack");
	for (auto& entry : fs::directory_iterator(rack_path.c_str())) {
		//std::cout << entry.path() << std::endl;
		GUIImage* image = &(rack_images_list[i]);
		bool ret = LoadTextureFromFile(entry.path().string().c_str(), &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		image->my_image_width *= 0.9f;
		image->my_image_height *= 0.9f;
		i++;
	}
	rack_image_idx = 1;
	
	i = 0;
	GUIImage icon_images_list[NUM_ICON];
	const std::string icon_path = picture_dir + std::string("/icon");
	for (auto& entry : fs::directory_iterator(icon_path.c_str())) {
		GUIImage* image = &(icon_images_list[i]);
		bool ret = LoadTextureFromFile(entry.path().string().c_str(), &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		image->my_image_width *= 2.0f;
		image->my_image_height *= 2.0f;
		i++;
	}
	icon_images_map = {
		{CHAR_BUMBUS, icon_images_list[0]},
		{CHAR_POGO, icon_images_list[1]},
		{CHAR_SWAINKY, icon_images_list[2]},
		{CHAR_GILMAN, icon_images_list[3]},
	};

	const std::string score_background_path = picture_dir + std::string("/score_background.png");
	LoadTextureFromFile(score_background_path.c_str(), &(score_background.my_image_texture),
	                    &(score_background.my_image_width), &(score_background.my_image_height));

	const std::string minimap_background_path = picture_dir + std::string("/minimap_background.png");
	LoadTextureFromFile(minimap_background_path.c_str(), &(minimap_background.my_image_texture),
		&(minimap_background.my_image_width), &(minimap_background.my_image_height));
	
	i = 0;
	const std::string fishy_path = picture_dir + std::string("/fishy");
	for (auto& entry : fs::directory_iterator(fishy_path.c_str())) {
		//std::cout << entry.path() << std::endl;
		GUIImage* image = &(fish_images_list[i]);
		bool ret = LoadTextureFromFile(entry.path().string().c_str(), &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		i++;
	}

	i = 0;
	const std::string tools_path = picture_dir + std::string("/tools");
	for (auto& entry : fs::directory_iterator(tools_path.c_str())) {
		//std::cout << entry.path() << std::endl;
		GUIImage* image = &(tool_images_list[i]);
		bool ret = LoadTextureFromFile(entry.path().string().c_str(), &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		i++;
	}

	const std::string curtain_path = picture_dir + std::string("/curtain-shed-side.png");
	LoadTextureFromFile(curtain_path.c_str(), &(curtain_img.my_image_texture),
	                    &(curtain_img.my_image_width), &(curtain_img.my_image_height));

	const std::string stamina_path = picture_dir + std::string("/stamina.png");
	LoadTextureFromFile(stamina_path.c_str(), &(stamina_image.my_image_texture),
	                    &(stamina_image.my_image_width), &(stamina_image.my_image_height));

	const std::string time_border_path = picture_dir + std::string("/time-border.png");
	LoadTextureFromFile(time_border_path.c_str(), &(timer_background.my_image_texture),
	                    &(timer_background.my_image_width), &(timer_background.my_image_height));
	
	timer_background.my_image_width *= 0.7f; 
	timer_background.my_image_height *= 0.7f;

	i = 0;
	std::string vegetable_path = picture_dir + std::string("/Vegetable");
	for (auto& entry : fs::directory_iterator(vegetable_path.c_str())) {
		//std::cout << entry.path() << std::endl;
		GUIImage* image = &(veg_images_list[i]);
		bool ret = LoadTextureFromFile(entry.path().string().c_str(), &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		image->my_image_height *= 10; 
		image->my_image_width *= 10;
		i++;
	}
	
	std::string sale_background_path = picture_dir + std::string("/sale_background.png");
	LoadTextureFromFile(sale_background_path.c_str(), &(sale_background.my_image_texture),
	                    &(sale_background.my_image_width), &(sale_background.my_image_height));

	i = 0;
	std::string characters_path = picture_dir + std::string("/characters");
	for (auto& entry : fs::directory_iterator(characters_path.c_str())) {
		//std::cout << entry.path() << std::endl;
		GUIImage* image = &(char_images_list[i]);
		bool ret = LoadTextureFromFile(entry.path().string().c_str(), &(image->my_image_texture),
			&(image->my_image_width), &(image->my_image_height));
		
		i++;
	}
}

void GUI::initializeLoadingImage() {
	const std::string loading_bg_path = picture_dir + std::string("/loading_background.png");
	LoadTextureFromFile(loading_bg_path.c_str(), &(loading_background.my_image_texture),
		&(loading_background.my_image_width), &(loading_background.my_image_height));


	int i = 0;
	const std::string chase_dir = picture_dir + std::string("/chasing");
	for (auto& entry : fs::directory_iterator(chase_dir.c_str())) {
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
	ImGui::PushFont(font_Ranchers);
	ImGui::Text("Loading: %d %c...", (int)(percent * 100), '%');
	ImGui::PopFont();
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
	if (GUI_show_buy_ui!=show) {
		rack_image_idx = 1;
		tool_image_idx = 1; 
		(&rack_images_list[1])->fade_ratio = 1;
		(&rack_images_list[1])->fade_in = true;

		curtain_img.fade_ratio = 0.001; 
		curtain_img.fade_in = true;
		sale_tools = false; 
	}
	GUI_show_buy_ui = show;
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
	ImVec2 window_size = ImVec2(width, height); 

	ImGui::SetNextWindowSize(window_size);
	ImGui::SetNextWindowPos(ImVec2(padding, window_height - padding), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::Begin("Minimap background", &bptr, TRANS_WINDOW_FLAG);
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Image((void*)(intptr_t)minimap_background.my_image_texture, image_size);
	ImGui::End(); 

	ImGui::SetNextWindowSize(window_size);
	ImGui::SetNextWindowPos(ImVec2(padding, window_height - padding), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	//ImVec2 center = ImVec2(padding + size.x / 2, window_height - padding - size.y / 2); // center of minimap
	ImVec2 center = ImVec2(image_size.x * 0.5f, image_size.y  * 0.5f); // center of minimap

	ImGui::Begin("MiniMap", &bptr, TRANS_WINDOW_FLAG);
	//place all player's icon:
	for (ModelEnum char_model: char_selections) {
		GUIImage image = icon_images_map[char_model];
		//TODO convert the pos into relative pos in minimap
		ImVec2 icon_size = ImVec2(image.my_image_width * display_ratio, image.my_image_height * display_ratio);

		const int i = char_model - CHAR_BUMBUS;
		ImVec2 relative_pos = ImVec2(player_pos[i].x / world_dim * image_size.x- icon_size.x * 0.5f, player_pos[i].y / world_dim * image_size.y - icon_size.y * 0.5f); 
		//size of icon 
		ImGui::SetCursorPos(relative_pos + center);
		ImGui::Image((void*)(intptr_t)image.my_image_texture, icon_size);
	}
	ImGui::End();
}

void GUI::createStamina() {
	bool bptr;
	const ImU32 col = IM_COL32(245.f, 61.f, 119.f, 250);//ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	const ImU32 bg = IM_COL32(227.f, 188.f, 208.f, 200); //ImGui::GetColorU32(ImGuiCol_Button);
	ImVec2 border_size = ImVec2(stamina_image.my_image_width * display_ratio, stamina_image.my_image_height * display_ratio);

	float top_padding = border_size.y * 0.1f ;
	float side_padding = border_size.x / 6.0f; 
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::SetNextWindowSize(border_size);
	ImGui::SetNextWindowPos(ImVec2(window_width- border_size.x, window_height), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::Begin("Stamina Bar", &bptr, TRANS_WINDOW_FLAG);
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Image((void*)(intptr_t)stamina_image.my_image_texture, border_size);
	ImGui::SetCursorPos(ImVec2(side_padding, top_padding-side_padding));
	ImGui::ReverseBufferingBar("##Stamina_bar", stamina_percent / 100, ImVec2(border_size.x-side_padding*2, border_size.y - top_padding), bg, col);
	ImGui::End();
	//ImGui::PopStyleVar(2);
}

void GUI::createTimer(float ratio) {
	float padding = 64.0f * display_ratio;
	int width = 560;
	int height = 560;
	ImVec2 size = ImVec2(width * display_ratio, height * display_ratio);
	const ImU32 col = IM_COL32(245.f, 61.f, 119.f, 255);//ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	const ImU32 bg = IM_COL32(227.f, 188.f, 208.f, 255); //ImGui::GetColorU32(ImGuiCol_Button);

	//float ratio = GUI_timer_percent;
	//std::cout << "the timer percent is " << ratio << std::endl;
	ImVec2 bg_size = ImVec2(timer_background.my_image_width * display_ratio, timer_background.my_image_height * display_ratio);
	ImGui::SetNextWindowPos(ImVec2(window_width - size.x,  size.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::SetNextWindowSize(bg_size);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin("Timer", NULL, TRANS_WINDOW_FLAG);
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Image((void*)(intptr_t)timer_background.my_image_texture, bg_size);
	ImGui::End();

	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(ImVec2(window_width - padding - size.x, padding + size.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::Begin("Timer", NULL, TRANS_WINDOW_FLAG);
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Spinner("##spinner", 200 * display_ratio, 80 * display_ratio, 1, col, 30);
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImVec2 center = ImGui::Spinner("##spinner", 200 * display_ratio, 80 * display_ratio, ratio, bg, 120);
	ImGui::End();
	ImGui::SetNextWindowPos(ImVec2(window_width - padding - size.x, padding + size.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::SetNextWindowSize(size);
	ImGui::Begin("Timer text", NULL, TRANS_WINDOW_FLAG);
	auto text_size = ImGui::CalcTextSize(GUI_timer_string.c_str());
	ImGui::SetCursorPos((size - text_size) * 0.5f);
	ImGui::Text(GUI_timer_string.c_str());
	ImGui::End();
	ImGui::PopStyleVar(2);

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
	ImGui::PushFont(font_Ranchers);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(51, 48, 49, 255));
	ImGui::Text("%d of %d client joined...", client_joined, max_client);
	ImGui::PopFont();
	ImGui::PopStyleColor();
	ImGui::End();

	// Rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(my_window);
}

void GUI::setTimer(float time, int r_sec) {
	timer_percent = time; 
	remaining_sec = r_sec; 
}

ModelEnum GUI::renderCharacterSelection(std::unordered_map<ModelEnum, int>& selected_chars, int client_idx) {
	ModelEnum res = SENTINEL_END;
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
		res = static_cast<ModelEnum>(CHAR_BUMBUS + char_selection_idx);
	} else if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
		for (int avail_idx = char_selection_idx - 1; avail_idx >= 0; avail_idx--)
		{
			if (selected_chars.count(static_cast<ModelEnum>(CHAR_BUMBUS + avail_idx)) == 0)
			{
				char_selection_idx = avail_idx;
				break;
			}
		}
	} else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
		for (int avail_idx = char_selection_idx + 1; avail_idx < NUM_ICON; avail_idx++)
		{
			if (selected_chars.count(static_cast<ModelEnum>(CHAR_BUMBUS + avail_idx)) == 0)
			{
				char_selection_idx = avail_idx;
				break;
			}
		}
	}
	
	ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Character Selection", NULL, TRANS_WINDOW_FLAG);
	ImVec2 cursor = ImVec2(10, 10);
	for (int i = 0; i < NUM_ICON; i++) {
		ModelEnum curr_icon_model = static_cast<ModelEnum>(CHAR_BUMBUS + i);
		ImGui::SetCursorPos(cursor);
		GUIImage image = char_images_list[i];
		ImVec2 image_size = ImVec2(image.my_image_width * display_ratio, image.my_image_height * display_ratio);
		ImGui::Image((void*)(intptr_t)image.my_image_texture, image_size);
		if (selected_chars.count(curr_icon_model) != 0) {
			std::string selected_text = "PLAYER " + std::to_string(selected_chars[curr_icon_model] + 1);
			auto text_size = ImGui::CalcTextSize(selected_text.c_str());
			ImGui::SetCursorPos(cursor + (image_size - text_size) * 0.5f);
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(51, 48, 49, 255));
			ImGui::Text(selected_text.c_str());
			ImGui::PopStyleColor();
		}
		if (i == char_selection_idx) {
			ImGui::SetCursorPos(cursor);
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			const ImU32 col = IM_COL32(245.f, 61.f, 119.f, 250);
			draw_list->AddRect(cursor, image_size + cursor, col, 0.0f, ImDrawFlags_None, 5.0f);
		}
		cursor.x += image_size.x + 10; 
	}
	ImGui::End();

	// Rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(my_window);
	
	return res; 
}

bool GUI::renderWinningScene() {
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (ImGui::IsKeyPressed(ImGuiKey_R)) {
		winning_fade_ratio = 1; 
	}
	winning_fade_ratio  = winning_fade_ratio < 0.0001? 0 : winning_fade_ratio* 0.9;
	float banner_width_ratio = 0.2f; 
	float ratio = 1 - winning_fade_ratio;
	ImVec2 size = ImVec2(window_width, window_height);
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

	ImGui::Begin("black banner", NULL, TRANS_WINDOW_FLAG);
	ImGui::SetCursorPos(ImVec2(0, 0));
	bool b = ImGui::BlackBanner("#top_black_banner", window_width, window_height, ratio, true, banner_width_ratio);
	ImGui::SetCursorPos(ImVec2(0, window_height * (1-banner_width_ratio * ratio)));
	ImGui::BlackBanner("#bot_black_banner", window_width, window_height, ratio, false, banner_width_ratio);
	ImGui::End();

	ImGui::PopStyleVar(2);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	return true; 
}

void GUI::createBuyConfirmation() {
	bool open_ptr = true;
	GUIImage veg_image = veg_images_list[veggie_sale_idx];

	//ImVec2 fish_size = ImVec2(window_width, window_width * sale_background.my_image_height / sale_background.my_image_width);
	ImVec2 fish_size = (1.0f* window_width) / window_height > (1.0f*sale_background.my_image_width) / sale_background.my_image_height ?
		ImVec2(window_height * sale_background.my_image_width / sale_background.my_image_height, window_height) :
		ImVec2(window_width, window_width * sale_background.my_image_height / sale_background.my_image_width);

	ImVec2 veg_size = ImVec2(veg_image.my_image_width * (fish_size.y * 0.5f) / veg_image.my_image_height, fish_size.y * 0.5f);
	//ImVec2 veg_size = ImVec2(veg_image.my_image_width * display_ratio, veg_image.my_image_height * display_ratio);

	ImVec2 window_size = ImVec2(window_width, window_height);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 255));
	ImGui::SetNextWindowSize(window_size);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("sale_UI_bg", &open_ptr, DRAK_WINDOW_FLAG);
	ImGui::End();

	ImGui::SetNextWindowSize(fish_size);
	ImGui::SetNextWindowPos((window_size - fish_size) * 0.5f);
	ImGui::Begin("Sale UI confirmation", &open_ptr, DRAK_WINDOW_FLAG);
	ImGui::SetScrollX(0); //prevent scroll cause the context is actually larger than the window 
	ImGui::SetScrollY(0);
	// add the fish layer
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Image((void*)(intptr_t)sale_background.my_image_texture, fish_size);

	ImGui::SetCursorPos(ImVec2(fish_size.x * 0.75 - veg_size.x *0.5, (fish_size.y - veg_size.y) * 0.5f));
	ImGui::Image((void*)(intptr_t)veg_image.my_image_texture, veg_size);

	ImGui::PushFont(font_Are_You_Serious);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(51, 48, 49, 255));

	//show talking box
	ImGui::SetCursorPos(ImVec2(fish_size.x * 0.125, fish_size.x * 0.125));
	ImGui::Text("%s! That is worth %f dollar(s)\n. Press [Enter] to sell!", seed_type_list[0], veg_sell_list[0]);
	ImGui::PopFont();
	ImGui::PopStyleColor(2);


	ImGui::End();
}






