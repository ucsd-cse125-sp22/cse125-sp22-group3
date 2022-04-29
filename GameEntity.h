#pragma once

enum class EntityType {
    PLAYER,
	VEGETABLE,
	PLOT,
	SEED,
	GROWTH
};

class GameEntity {
	public:
	virtual void FixedUpdate() = 0;
	EntityType type;
};
