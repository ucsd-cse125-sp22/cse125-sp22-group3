#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <unordered_map>

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
	STOP_SPRINT
};

enum ModelEnum
{
	CHAR_BUMBUS,
	CHAR_POGO,
	CHAR_SWAINKY,
	CHAR_GILMAN,

	VEG_CABBAGE,
	VEG_CARROT,
	VEG_CORN,
	VEG_RADISH,
	VEG_TOMATO, // do i name it veggie if a tomato is not a veggie hrmmmm thimking

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

	WORLD_MAP
};

enum AniMode {
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

static std::unordered_map<VegetableType, VeggieInfo> veggie_map = {
	{VegetableType::TOMATO, VeggieInfo{VEG_TOMATO, WORLD_FLAG_TOMATO, WORLD_SEED_TOMATO, 5, 10, 50, true, true}},
	{VegetableType::CABBAGE, VeggieInfo{VEG_CABBAGE, WORLD_FLAG_CABBAGE, WORLD_SEED_CABBAGE, 5, 2, 6, true, false}},
	{VegetableType::CORN, VeggieInfo{VEG_CORN, WORLD_FLAG_CORN, WORLD_SEED_CORN, 25, 2, 10, false, false}},
	{VegetableType::CARROT, VeggieInfo{VEG_CARROT, WORLD_FLAG_CARROT, WORLD_SEED_CARROT, 5, 1, 5, false, false}},
	{VegetableType::RADISH, VeggieInfo{VEG_RADISH, WORLD_FLAG_RADISH, WORLD_SEED_RADISH, 5, 5, 20, false, true}}
};

#endif