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

static float stamina_percent; //use this variable to set the stamina display


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
	// buying
	BUY_CARROT,
	BUY_CABBAGE,
	BUY_CORN,
	BUY_RADISH,
	BUY_TOMATO,
	// selling
	SELL_CMD,
	CLOSE_UI
};

enum ModelEnum
{
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

	POISON,
	WATERING_CAN,
	SHOVEL,
	HOE,
	NET,
	FERTILIZER,
	GLUE,

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

	WORLD_MAP,

	PARTICLE_GLOW,
	PARTICLE_DUST
};

enum AniMode {
	DANCE,
	IDLE_HOLD,
	IDLE_WALK,
	IDLE,
	WALK,
	NO_ANI
};

enum class VegetableType {
	TOMATO,
	CABBAGE,
	CORN,
	CARROT,
	RADISH,
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

// veggie model, // model flag, seed model , growth time, seed price, sell price, requires water? requires fertilizer?
static std::unordered_map<VegetableType, VeggieInfo> veggie_map = {
	{VegetableType::TOMATO, VeggieInfo{VEG_TOMATO, WORLD_FLAG_TOMATO, WORLD_SEED_TOMATO, 5, 10, 50, true, true}},
	{VegetableType::CABBAGE, VeggieInfo{VEG_CABBAGE, WORLD_FLAG_CABBAGE, WORLD_SEED_CABBAGE, 5, 2, 6, true, false}},
	{VegetableType::CORN, VeggieInfo{VEG_CORN, WORLD_FLAG_CORN, WORLD_SEED_CORN, 25, 2, 10, false, false}},
	{VegetableType::CARROT, VeggieInfo{VEG_CARROT, WORLD_FLAG_CARROT, WORLD_SEED_CARROT, 5, 1, 5, false, false}},
	{VegetableType::RADISH, VeggieInfo{VEG_RADISH, WORLD_FLAG_RADISH, WORLD_SEED_RADISH, 5, 5, 20, false, true}}
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
#endif