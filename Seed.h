#pragma once

#include "Vegetable.h"
#include "Holdable.h"
#include "Interactable.h"

class Seed : public Interactable, public Holdable, public PhysicsObject, public GameEntity, public Drawable {
	
	private:
		VegetableType type;
		ModelEnum model;

		ColliderCircle* collider_;
		bool isPlanted = false;
		// Transformations
		glm::vec2* translate = nullptr;
		glm::mat4 rotation{};
		glm::mat4 hold_transformation_{};

		bool isHoldable = true;
		bool isHeld = false;

	public:
		AniMode modelAnim = NO_ANI;

		Seed(VegetableType type, ModelEnum curr);
		~Seed();

		void FixedUpdate() override;

		// PhysicsObject
		void OnCollide(PhysicsObject* object) override {};
		void OnTrigger(PhysicsObject* object) override {};
		std::vector<Collider*> GetColliders() override;
		glm::vec2* GetWorldPosition() override;

		// Interactable
		bool CanInteract(Player* player) override;
		void OnInteract(Player* player) override;
		void OnDrop() override;
		glm::mat4 GetHoldTransform() override;
		void SetPlanted();

		// Drawable
		glm::mat4 GetParentTransform() override;
		ModelEnum GetModelEnum() override;
		AniMode GetAniMode() override;

		// Information
		void SetModel(ModelEnum newModel, glm::vec2 pos);
		glm::mat4 GetTransformation();
		glm::mat4 GetTranslation();
		glm::vec3 GetPosition() const;
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::mat4 rotation);
		glm::mat4 GetRotation() const;
		VegetableType GetType();

		// Growth stuff
		double plantedTime = 0;
		bool getIsReady();
		bool isHarvestable = false;

		
};
