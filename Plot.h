#pragma once

#include "Interactable.h"
#include "Vegetable.h"
#include "Model.h"

class Plot : public Interactable, public Drawable, public PhysicsObject, public GameEntity {
	public:
		Plot(Model curr);
		~Plot();

		// GameEntity
		void FixedUpdate() override;

		// Drawable
		void Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) override;
		
		// PhysicsObject
        void OnCollide(PhysicsObject* object) override {};
		void OnTrigger(PhysicsObject* object) override {};
        std::vector<Collider*> GetColliders() override;
        glm::vec2* GetWorldPosition() override;

		// Information
		glm::vec3 GetPosition() const;
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::mat4 rotation);
		bool isPlanted = false;
		void SetPlantedVegetable(Vegetable* vegetable);
		Vegetable* GetPlantedVegetable();


		// Interactable
		bool CanInteract(Player* player) override;
		void OnInteract(Player* player) override;

	private:
		// Transformations
		glm::vec2* translate = nullptr;
		glm::mat4 rotation{};

		// Current model to display
		Model model;

		ColliderCircle* collider_;

		// Get matrix transformation
		glm::mat4 GetTransformation();

		Vegetable* plantedVegetable = nullptr;
};