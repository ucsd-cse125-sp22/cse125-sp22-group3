#ifndef _PLAYER_H_
#define _PLAYER_H_


#include "Model.h"

class Player {
	private:
		const float RUN_OFFSET = 20;
		const float TURN_OFFSET = 10;

		// Keep track of time for movement
		float currTime;
		float lastTime;

		// Transformations for our player
		glm::vec3 translate;
		glm::vec3 rotate;
		glm::vec3 scale;

		// speed and turn of player
		float currTurn = 0.f;
		float currSpeed = 0.f;

		// Current model to display
		Model* current;
		Model* idle;
		Model* walking;

		// Moving player
		void move();
		bool backward;
		bool forward;
		bool lastMove; // 0 - backward, 1 - forward

		bool left;
		bool right;
		bool lastTurn; // 0 - left, 1 - right

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
		void update();
		void draw(glm::mat4 view, glm::mat4 projection, GLuint shader);

		// Movement
		void Forward();
		void Backward();
		void Left();
		void Right();
		void StopMovingForward();
		void StopMovingBackward();
		void StopTurningLeft();
		void StopTurningRight();
};

#endif
