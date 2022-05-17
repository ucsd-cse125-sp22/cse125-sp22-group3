#pragma once
#include "ColliderCircle.h"
#include "Drawable.h"
#include "GameEntity.h"
#include "Holdable.h"
#include "Interactable.h"
#include "PhysicsObject.h"
#include "Player.h"

class GardenTool : public Interactable, public Holdable, public PhysicsObject, public GameEntity, public Drawable {

private:
	ModelEnum model;

	ColliderCircle* collider_;

	// Transformations
	glm::vec2* translate = nullptr;
	glm::vec3* translate3D = nullptr;
	glm::mat4 rotation{};

public:
	AniMode modelAnim = NO_ANI;

	GardenTool(ModelEnum curr);
	~GardenTool();

	void FixedUpdate() override;

	// PhysicsObject
	void OnCollide(PhysicsObject* object) override {};
	void OnTrigger(PhysicsObject* object) override {};
	std::vector<Collider*> GetColliders() override;
	glm::vec2* GetWorldPosition() override;

	// Drawable
	glm::mat4 GetParentTransform() override;
	ModelEnum GetModelEnum() override;
	AniMode GetAniMode() override;

	// Information
	void SetModel(ModelEnum newModel, glm::vec3 pos);
	glm::mat4 GetTransformation();
	glm::mat4 GetTranslation();
	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 position);
	void SetRotation(glm::mat4 rotation);
	glm::mat4 GetRotation() const;
	bool isHeld = false;
	Player* holding_player = nullptr;

	// translation for player interact;
	void SetHeight(float height);
	float GetHeight();
};

