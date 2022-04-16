#pragma once

enum class EntityType {
    PLAYER,
	VEGETABLE
};

class GameEntity {
	public:
	virtual void FixedUpdate() = 0;
	EntityType type;
};
