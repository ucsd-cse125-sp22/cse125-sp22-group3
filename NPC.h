#pragma once
#include "Drawable.h"
#include "GameEntity.h"
#include "PhysicsObject.h"
#include "Interactable.h"

class NPC : public Drawable, public GameEntity, public PhysicsObject, public Interactable {
public:
	NPC(ModelEnum curr);
	~NPC();

	// current animation, no custom get/set logic so is set as public field
	AniMode modelAnim;

	// GameEntity
	void FixedUpdate() override;

	// Drawable
	glm::mat4 GetParentTransform() override;
	ModelEnum GetModelEnum() override;
	AniMode GetAniMode() override;

	// PhysicsObject
	void OnCollide(PhysicsObject* object) override {}
	void OnTrigger(PhysicsObject* object) override {};
	std::vector<Collider*> GetColliders() override;
	glm::vec2* GetWorldPosition() override;

	// Information
	glm::vec3 GetPosition() const;
	void SetWorldPosition(glm::vec3 position);

	// Interactable
	bool CanInteract(Player* player) override;
	void OnInteract(Player* player) override;

private:
	// Transformations
	glm::vec2* translate = nullptr;
	glm::mat4 rotation{};	
	glm::vec3 scale;

	// Current model to display
	ModelEnum model;

	// Current Collider
	ColliderCircle* collider_;

	// Get matrix transformationa
	glm::mat4 GetRotation();
	glm::mat4 GetTranslation();
	glm::mat4 GetScale();

};