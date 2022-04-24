#pragma once

enum class EntityType {
    PLAYER,
	VEGETABLE,
	PLOT
};

class GameEntity {
	public:
	virtual void FixedUpdate() = 0;
	EntityType type;
};
