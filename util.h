#pragma once
#ifndef UTIL_H
#define UTIL_H
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
	DROP
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

	WORLD_SEED,
	WORLD_PLOT
};

enum AniMode {
	IDLE_HOLD,
	IDLE_WALK,
	IDLE,
	WALK,
	NO_ANI
};

#endif