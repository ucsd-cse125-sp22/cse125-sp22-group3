#pragma once

#include "Drawable.h"
#include "GameEntity.h"
#include "PhysicsObject.h"
#include "ColliderCircle.h"

class World : public GameEntity, public Drawable, public PhysicsObject {
	private:
		ModelEnum model;
		ColliderCircle* collider_;

		// Transformations
		glm::vec2* translate = nullptr;

	public:
		AniMode modelAnim = NO_ANI;

		// Constructor / Deconstructor
		World(ModelEnum model);
		~World();

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
		void SetModel(ModelEnum newModel, glm::vec2 pos);
		glm::mat4 GetTranslation();
		glm::vec3 GetPosition() const;
		void SetPosition(glm::vec3 position);
};