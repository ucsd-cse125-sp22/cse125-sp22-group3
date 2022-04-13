#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "Drawable.h"
#include "Model.h"

class Player : public Drawable {
	private:
		// Transformations for our player
		glm::vec3 translate;
		glm::vec3 rotate;
		glm::vec3 scale;

		// Player Movement Attributes
		const float friction_ = 50;
		const float base_accel_ = 200;
		const float max_velocity_ = 20;
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
		void Update();
		void draw(glm::mat4 view, glm::mat4 projection, GLuint shader);

		// Movement
		glm::vec2 move_input{0,0};

		// Information
		glm::vec3 GetPosition() const;

};

#endif
