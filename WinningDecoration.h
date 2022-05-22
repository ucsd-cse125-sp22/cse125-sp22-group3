#pragma once
#include "Drawable.h"
#include "GameEntity.h"

class WinningDecoration : public Drawable, public GameEntity
{
public:
	WinningDecoration(ModelEnum curr);
	~WinningDecoration();

	// Drawable
	ModelEnum GetModelEnum() override;
	AniMode GetAniMode() override;
	glm::mat4 GetParentTransform() override;

	// GameEntity
	void FixedUpdate() override;

	// Information
	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 position);
	glm::mat4 GetRotation() const;
	void SetRotation(glm::mat4 rotation);
	void SetHeight(float height);
	float GetHeight();
	float upVelocity = 0;
	float currHeight = 0;

private:
	// Transformations
	glm::vec2* translate = nullptr;
	glm::vec3* translate3D = nullptr;
	glm::mat4 rotation{};
	glm::mat4 hold_transformation_{};

	// Current model to display
	ModelEnum model;
	AniMode modelAnim;

	// Get matrix transformation
	glm::mat4 GetTransformation();
	glm::mat4 GetTranslation();

	float pickupHeight = -1;
	float dropHeight = -3;
};

