#pragma once

#include "ColliderCircle.h"
#include "Drawable.h"
#include "GameEntity.h"
#include "Interactable.h"
#include "Harvestable.h"
#include "Holdable.h"
#include "PhysicsObject.h"
#include "Player.h"

class Vegetable : public Interactable, public PhysicsObject, public Drawable, public GameEntity, public Holdable {
    public:
        // current animation, no custom get/set logic so is set as public field
        AniMode modelAnim;

        Vegetable(VegetableType type, ModelEnum curr);
        ~Vegetable();
        VegetableType type = VegetableType::NOVEGGIE;

        // GameEntity
        void FixedUpdate() override;

        // Drawable
        glm::mat4 GetParentTransform() override;
        ModelEnum GetModelEnum() override;
		AniMode GetAniMode() override;
		

        // PhysicsObject
        void OnCollide(PhysicsObject* object) override {};
		void OnTrigger(PhysicsObject* object) override {}
	
        std::vector<Collider*> GetColliders() override;
        glm::vec2* GetWorldPosition() override;

        // Information
        glm::vec3 GetPosition() const;
        void SetPosition(glm::vec3 position);
        void SetRotation(glm::mat4 rotation);
        glm::mat4 GetRotation() const;

        // Interactable
        bool CanInteract(Player* player) override;
        void OnInteract(Player* player) override;
		void OnDrop() override;
		glm::mat4 GetHoldTransform() override;
        bool isHoldable = true;
		bool isHeld = false;
		Player* holding_player = nullptr;

        void SetHeight(float height);
        float GetHeight();

	private:
		// Transformations
		glm::vec2* translate = nullptr;
        glm::vec3* translate3D = nullptr;
        glm::mat4 rotation{};
		glm::mat4 hold_transformation_{};

		// Current model to display
		ModelEnum model; //TODO Change ModelEnum to some other new DisplayData/Model struct

		ColliderCircle* collider_;

		// Get matrix transformation
		glm::mat4 GetTransformation();
        glm::mat4 GetTranslation();

        float pickupHeight = -1;
        float dropHeight = -3;

};
