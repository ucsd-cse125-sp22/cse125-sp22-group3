#pragma once
#include "ColliderCircle.h"
#include "Drawable.h"
#include "GameEntity.h"
#include "Model.h"
#include "PhysicsObject.h"

class Player : public Drawable, public GameEntity, public PhysicsObject {
	private:
		// Transformations for our player
		glm::vec2* translate = nullptr;
		glm::vec3 rotate;
		glm::vec3 scale;

		// Player Movement Attributes
		float friction_ = 50; // Resistance in Units/Sec
		float base_accel_ = 200; // Acceleration in Units/Sec^2
		float max_velocity_ = 20; // Max Velocity in Units/Sec
		glm::vec2 curr_vel_ = glm::vec3(0,0,0);

		// Current model to display
		Model* model = nullptr;

		// Current Player Collider
		// Player will use a Circle Collider
		ColliderCircle* collider_;

		// Moving player
		void Move();
	
		// Get matrix transformationa
		glm::mat4 GetRotation();
		glm::mat4 GetTranslation();
		glm::mat4 GetScale();

	public:
		Player();

		/* Curr should be idle */
		Player(Model * curr);

		// GameEntity
		void FixedUpdate() override;

		// Drawable
		void Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) override;

		// PhysicsObject
		void OnCollide() override {};
		Collider* GetCollider() override;
		glm::vec2* GetWorldPosition() override;
		
		// Movement
		glm::vec2 move_input{0,0};

		// Information
		glm::vec3 GetPosition() const;
		void SetPosition(glm::vec3 position);

};