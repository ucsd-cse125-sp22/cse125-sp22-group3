#pragma once

enum class EntityType {
    PLAYER,
	VEGETABLE,
	PLOT,
	SEED,
	GROWTH,
	WORLD
};

class GameEntity {
	public:
	virtual void FixedUpdate() = 0;
};
