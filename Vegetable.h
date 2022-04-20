#pragma once

#include "Interactable.h"
#include "Harvestable.h"

enum class VegetableType {
    TOMATO,
    CABBAGE,
    CORN,
    CARROT,
    RADISH
};

class Vegetable : public Interactable, public PhysicsObject, public Drawable, public GameEntity {
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

        // Interactable
        bool CanInteract(Player* player) override;
        void OnInteract(Player* player) override;
        bool isHoldable = true;

	private:
		// Transformations
		glm::vec2* translate = nullptr;

		// Current model to display
		Model model;

		ColliderCircle* collider_;

		// Get matrix transformationa
		glm::mat4 GetTranslation();
};