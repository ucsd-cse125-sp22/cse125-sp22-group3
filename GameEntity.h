#pragma once

enum class EntityType {
    PLAYER,
	VEGETABLE,
	PLOT,
	SEED
};

class GameEntity {
	public:
	virtual void FixedUpdate() = 0;
};
