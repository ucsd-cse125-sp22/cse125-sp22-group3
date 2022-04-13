#pragma once
#include "Drawable.h"
#include "GameEntity.h"
#include "Model.h"

class Player : public Drawable, public GameEntity {
	private:
		// Transformations for our player
		glm::vec3 translate;
		glm::vec3 rotate;
		glm::vec3 scale;

		// Player Movement Attributes
		float friction_ = 50; // Resistance in Units/Sec
		float base_accel_ = 200; // Acceleration in Units/Sec^2
		float max_velocity_ = 20; // Max Velocity in Units/Sec
		glm::vec3 curr_vel_ = glm::vec3(0,0,0);

		// Current model to display
		Model* current;
		Model* idle;
		Model* walking;

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

		// Animations / Modes
		void addIdle(Model* idle);
		void addWalking(Model* walking);

		// Rendering
		void FixedUpdate() override;
		void Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) override;

		// Movement
		glm::vec2 move_input{0,0};

		// Information
		glm::vec3 GetPosition() const;
		void SetPosition(glm::vec3 position);

};