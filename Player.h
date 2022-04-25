#pragma once

#include "glm.h"
#include "ColliderCircle.h"
#include "Drawable.h"
#include "GameEntity.h"
#include "PhysicsObject.h"
#include "util.h"

class Player : public Drawable, public GameEntity, public PhysicsObject {
	private:
		// Transformations for our player
		glm::vec2* translate = nullptr;
		glm::vec3 rotate;
		glm::vec3 scale;

		// Player Movement Attributes
		float friction_ = 50.f; // Resistance in Units/Sec
		float base_accel_ = 200.f; // Acceleration in Units/Sec^2
		float max_velocity_ = 20.f; // Max Velocity in Units/Sec
		float entityHeldDist = 2.5f; // distance of entity from player
		glm::vec2 curr_vel_ = glm::vec2(0,0);

		// Current model to display
		ModelEnum model;

		// Currently holding
		GameEntity* entityHeld = nullptr;
		GameEntity* entityTriggered = nullptr;

		// Current Player Collider
		// Player will use a Circle Collider
		ColliderCircle* collider_;

		// Moving player
		void Move();
		void MoveHeld();
		void SetHoldEntity(GameEntity* entity);
		bool isHolding = false;
		void SetTriggeringEntity(GameEntity* entity); // Set the game object we're colliding with
		GameEntity* GetTriggeringEntity();
		GameEntity* GetHoldEntity();
	
		// Get matrix transformationa
		glm::mat4 GetRotation();
		glm::mat4 GetTranslation();
		glm::mat4 GetScale();

	public:
		// current animation, no custom get/set logic so is set as public field
		AniMode modelAnim;

		Player();

		/* Curr should be idle */
		Player(ModelEnum curr);

		// GameEntity
		void FixedUpdate() override;

		// Drawable
		void Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) override;
		void Draw(GLuint shader) override;
		glm::mat4 GetParentTransform();
		ModelEnum GetModel();

		// PhysicsObject
		void OnCollide(PhysicsObject* object) override {}
		void OnTrigger(PhysicsObject* object) override;
		std::vector<Collider*> GetColliders() override;
		glm::vec2* GetWorldPosition() override;
		
		// Input
		glm::vec2 move_input{0,0};
		void Use();
		void Drop();

		// Information
		glm::vec3 GetPosition() const;
		void SetWorldPosition(glm::vec3 position);
		bool GetIsHolding() const { return isHolding; }

};