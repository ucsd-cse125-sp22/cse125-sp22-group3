#pragma once
#include "Holdable.h"
#include "Interactable.h"
#include "GameManager.h"
class GlueOnGround : public PhysicsObject, public GameEntity, public Drawable
{

private:
	ModelEnum model = GLUE_ON_GROUND;

	ColliderCircle* collider_;

	// Transformations
	glm::vec2* translate = nullptr;
	glm::vec3* translate3D = nullptr;
	glm::mat4 rotation{};

public:
	AniMode modelAnim = NO_ANI;

	GlueOnGround();
	~GlueOnGround();

	void FixedUpdate() override;

	// PhysicsObject
	void OnCollide(PhysicsObject* object) override {};
	void OnTrigger(PhysicsObject* object) override; 
	std::vector<Collider*> GetColliders() override;
	glm::vec2* GetWorldPosition() override;

	// Drawable
	glm::mat4 GetParentTransform() override;
	ModelEnum GetModelEnum() override;
	AniMode GetAniMode() override;

	// Information
	glm::mat4 GetTransformation();
	glm::mat4 GetTranslation();
	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 position);
	void SetRotation(glm::mat4 rotation);
	glm::mat4 GetRotation() const;

	// glue info
	Player* ownerOfGlue = nullptr;
	Player* gluedPlayer = nullptr;
	float glueTime = 0;
	float maxGlueTime = 10;
};

