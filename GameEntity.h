#pragma once

enum class EntityType {
    PLAYER,
	VEGETABLE,
	PLOT,
	SEED,
	GROWTH,
	WORLD,
	PARTICLE
};

class GameEntity {
	public:
	virtual void FixedUpdate() = 0;
};
