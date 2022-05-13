#pragma once

#include "Interactable.h"
#include "Vegetable.h"
#include "Seed.h"

class Plot : public Interactable, public Drawable, public PhysicsObject, public GameEntity {
	public:
		// current animation, no custom get/set logic so is set as public field
		AniMode modelAnim;
		Plot(ModelEnum curr);
		~Plot();

		// GameEntity
		void FixedUpdate() override;

		// Drawable
		glm::mat4 GetParentTransform() override;
		ModelEnum GetModelEnum() override;
		AniMode GetAniMode() override;
		
		// PhysicsObject
        void OnCollide(PhysicsObject* object) override {};
		void OnTrigger(PhysicsObject* object) override {};
        std::vector<Collider*> GetColliders() override;
        glm::vec2* GetWorldPosition() override;

		// Information
		glm::vec3 GetPosition() const;
		ModelEnum GetModel();
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::mat4 rotation);
		bool isPlanted = false;
		void SetPlantedVegetable(Seed* seed);
		VegetableType GetPlantedVegetable();
		glm::vec3 GetTranslate();


		// Interactable
		bool CanInteract(Player* player) override;
		void OnInteract(Player* player) override;

		// Planting stuff
		Seed* plantedVegetable = nullptr;

	private:
		// Transformations
		glm::vec2* translate = nullptr;
		glm::vec3* translate3D = nullptr;
		glm::mat4 rotation{};

		// Current model to display
		ModelEnum model;

		ColliderCircle* collider_;

		// Get matrix transformation
		glm::mat4 GetTransformation();
};