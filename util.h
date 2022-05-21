#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <unordered_map>

static bool GUI_show_sale_ui; //use this to tigger the sale UI display 
static bool GUI_show_scoreboard; 
static bool GUI_show_minimap; 
static bool GUI_show_stamina;
static bool GUI_show_timer; 
static bool GUI_show_buy_ui; 

#define GAME_TIME_MINS 15

enum InputCommands {
	NONE,
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_LEFT,
	MOVE_RIGHT,
	STOP_FORWARD,
	STOP_BACKWARD,
	STOP_LEFT,
	STOP_RIGHT,
	USE,
	DROP,
	SPRINT,
	STOP_SPRINT,
	DANCE_CMD,
	// buying veggies
	BUY_CARROT,
	BUY_CABBAGE,
	BUY_CORN,
	BUY_RADISH,
	BUY_TOMATO,
	// buying tools
	BUY_NET,
	BUY_HOE ,
	BUY_WATER ,
	BUY_FERTILIZER ,
	BUY_SHOVEL ,
	BUY_GLUE ,
	BUY_POISON ,
	BUY_OATS ,
	BUY_SOJU ,
	
	// selling
	SELL_CMD,
	CLOSE_UI
};

enum ModelEnum
{
	SENTINEL_BEGIN, // ALWAYS has to be the FIRST enum

	WORLD_MAP,
	WORLD_WATER,
	WORLD_LEAVES,
	WORLD_GRASS,
	WORLD_PODIUM,
	
	CHAR_BUMBUS,
	CHAR_POGO,
	CHAR_SWAINKY,
	CHAR_GILMAN,
	CHAR_NPC, // i dont think we named her question mark

	VEG_CABBAGE,
	VEG_CARROT,
	VEG_CORN,
	VEG_RADISH,
	VEG_TOMATO, // do i name it veggie if a tomato is not a veggie hrmmmm thimking
	VEG_GOLDEN_EGGPLANT,

	POISON,
	WATERING_CAN,
	SHOVEL,
	HOE,
	NET,
	FERTILIZER,
	GLUE,
	GLUE_ON_GROUND,
	OATS,
	SOJU,

	WORLD_SEED_CABBAGE,
	WORLD_SEED_CARROT,
	WORLD_SEED_CORN,
	WORLD_SEED_TOMATO,
	WORLD_SEED_RADISH,

	WORLD_PLOT_RED,
	WORLD_PLOT_BLUE,
	WORLD_PLOT_GREEN,
	WORLD_PLOT_YELLOW,

	WORLD_FLAG_CABBAGE,
	WORLD_FLAG_CARROT,
	WORLD_FLAG_CORN,
	WORLD_FLAG_RADISH,
	WORLD_FLAG_TOMATO,
	WORLD_FLAG_POISON,

	INDICATOR_WATER,
	INDICATOR_FERTILIZER,

	DEBUG_CIRCLE,
	DEBUG_SQUARE,

	PARTICLE_GLOW,
	PARTICLE_DUST,

	BALLOON_YELLOW,
	BALLOON_RED,
	BALLOON_BLUE,
	BALLOON_PURPLE,
	BALLOON_PINK,
	BALLOON_GREEN,

	SENTINEL_END, // ALWAYS has to be the LAST enum
};

enum AniMode {
	DANCE,
	IDLE_HOLD,
	IDLE_WALK,
	IDLE,
	WALK,
	NO_ANI,
	PARTICLE_PLAY,
	PARTICLE_STOP
};

enum class VegetableType {
	TOMATO,
	CABBAGE,
	CORN,
	CARROT,
	RADISH,
	GOLDEN_EGGPLANT,
	NOVEGGIE
};

struct VeggieInfo {
	ModelEnum veggie_model;
	ModelEnum flag_model;
	ModelEnum seed_model;
	float growth_time;
	float seed_price;
	float sell_price;
	bool requires_water = false;
	bool requires_fertilizer = false;
};

struct ToolInfo {
	ModelEnum tool_model;
	float price;
};

// veggie model, // model flag, seed model , growth time, seed price, sell price, requires water? requires fertilizer?
static std::unordered_map<VegetableType, VeggieInfo> veggie_map = {
	{VegetableType::TOMATO, VeggieInfo{VEG_TOMATO, WORLD_FLAG_TOMATO, WORLD_SEED_TOMATO, 5, 10, 50, true, true}},
	{VegetableType::CABBAGE, VeggieInfo{VEG_CABBAGE, WORLD_FLAG_CABBAGE, WORLD_SEED_CABBAGE, 5, 2, 6, true, false}},
	{VegetableType::CORN, VeggieInfo{VEG_CORN, WORLD_FLAG_CORN, WORLD_SEED_CORN, 25, 2, 10, false, false}},
	{VegetableType::CARROT, VeggieInfo{VEG_CARROT, WORLD_FLAG_CARROT, WORLD_SEED_CARROT, 5, 1, 5, false, false}},
	{VegetableType::RADISH, VeggieInfo{VEG_RADISH, WORLD_FLAG_RADISH, WORLD_SEED_RADISH, 5, 5, 20, false, true}},
	{VegetableType::GOLDEN_EGGPLANT, VeggieInfo{VEG_GOLDEN_EGGPLANT, VEG_GOLDEN_EGGPLANT, VEG_GOLDEN_EGGPLANT, 0, 0, 100, false, false}}
};

static std::unordered_map<ModelEnum, ToolInfo> tool_map = {
	{ModelEnum::NET, ToolInfo{NET, 10}},
	{ModelEnum::HOE, ToolInfo{HOE, 5}},
	{ModelEnum::WATERING_CAN, ToolInfo{WATERING_CAN, 20}},
	{ModelEnum::FERTILIZER, ToolInfo{FERTILIZER, 2}},
	{ModelEnum::SHOVEL, ToolInfo{SHOVEL, 5}},
	{ModelEnum::GLUE, ToolInfo{GLUE, 3}},
	{ModelEnum::POISON, ToolInfo{POISON, 2}},
	{ModelEnum::OATS, ToolInfo{OATS, 5}},
	{ModelEnum::SOJU, ToolInfo{SOJU, 6}},
	
};

static std::unordered_map<ModelEnum, ModelEnum> plot_ownership = {
	{WORLD_PLOT_RED, CHAR_BUMBUS},
	{WORLD_PLOT_YELLOW, CHAR_POGO},
	{WORLD_PLOT_BLUE, CHAR_GILMAN},
	{WORLD_PLOT_GREEN, CHAR_SWAINKY},
	{CHAR_BUMBUS, WORLD_PLOT_RED},
	{CHAR_POGO, WORLD_PLOT_YELLOW},
	{CHAR_GILMAN, WORLD_PLOT_BLUE},
	{CHAR_SWAINKY, WORLD_PLOT_GREEN}
};

static float maxGlueTime = 10;

static glm::mat3 util_RotateAroundAxis(const float degrees, const glm::vec3& axis)
{
	float deg = glm::radians(degrees);
	glm::mat3 one = cosf(deg) * glm::mat3(1.0);
	glm::mat3 two = (1 - cosf(deg)) * (glm::mat3(axis, glm::vec3(), glm::vec3()) * glm::mat3(axis[0], 0, 0, axis[1], 0, 0, axis[2], 0, 0));
	glm::mat3 three = sinf(deg) * (glm::mat3(0, axis[2], -axis[1], -axis[2], 0, axis[0], axis[1], -axis[0], 0));
	return one + two + three;
}

static glm::mat2 util_Rotate2D(const float degrees)
{
	return glm::mat2(cosf(degrees),sinf(degrees),-sinf(degrees),cosf(degrees));
}

#endif