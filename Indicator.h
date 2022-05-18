#pragma once

#include "GameEntity.h"
#include "Drawable.h"

class Indicator : public GameEntity, public Drawable {
public:
	AniMode modelAnim = NO_ANI;

	Indicator(VegetableType veggie);
	~Indicator();

	// GameEntity
	void FixedUpdate() override;

	// Drawable
	glm::mat4 GetParentTransform() override;
	ModelEnum GetModelEnum() override;
	AniMode GetAniMode() override;

	// Information
	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 position);
	void SetRotation(glm::mat4 rotation);
	glm::mat4 GetRotation() const;
	void SetWater();
	void SetFertilize();


private:
	// Transformations
	glm::vec2* translate = nullptr;
	glm::vec3* translate3D = nullptr;
	glm::mat4 rotation{};

	ModelEnum model;

	// Get matrix transformation
	glm::mat4 GetTransformation();
	glm::mat4 GetTranslation();
	VegetableType type;

	float maxHeight = 4;
	float minHeight = 3;
	float currHeight = 3;
	bool goingUp = false;
	float heightIncrement = 0.01;
};