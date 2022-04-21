#pragma once

#include "Interactable.h"
#include "Harvestable.h"
#include "Holdable.h"

enum class VegetableType {
    TOMATO,
    CABBAGE,
    CORN,
    CARROT,
    RADISH
};

class Vegetable : public Interactable, public PhysicsObject, public Drawable, public GameEntity, public Holdable {
    public:
        Vegetable(VegetableType type, Model curr);
        ~Vegetable();
        VegetableType type;

        // GameEntity
        void FixedUpdate() override;

        // Drawable
        void Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) override;

        // PhysicsObject
        void OnCollide(PhysicsObject* object) override {};
		void OnTrigger(PhysicsObject* object) override {}
	
        std::vector<Collider*> GetColliders() override;
        glm::vec2* GetWorldPosition() override;

        // Information
        glm::vec3 GetPosition() const;
        void SetPosition(glm::vec3 position);
        void SetRotation(glm::mat4 rotation);

        // Interactable
        bool CanInteract(Player* player) override;
        void OnInteract(Player* player) override;
		void OnDrop() override;
		glm::mat4 GetHoldTransform() override;
        bool isHoldable = true;
		bool isHeld = false;

	private:
		// Transformations
		glm::vec2* translate = nullptr;
        glm::mat4 rotation{};
		glm::mat4 hold_transformation_{};

		// Current model to display
		Model model;

		ColliderCircle* collider_;

		// Get matrix transformation
		glm::mat4 GetTransformation();
};