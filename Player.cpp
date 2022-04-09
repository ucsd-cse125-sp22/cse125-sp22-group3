#include "Player.h"

Player::Player() {
	translate = glm::vec3(0.0f);
	rotate = glm::vec3(0.0f);
	scale = glm::vec3(0.0f);

	forward = false;
	backward = false;
	left = false;
	right = false;;
}

Player::Player(Model * curr) {
	// Set initial values
	current = curr;
	idle = curr;

	translate = glm::vec3(0.0f);
	rotate = glm::vec3(0.0f);
	scale = glm::vec3(0.0f);

	forward = false;
	backward = false;
	left = false;
	right = false;;
}

void Player::addIdle(Model* i) {
	idle = i;
}

void Player::addWalking(Model* w) {
	walking = w;
}

void Player::update() {
	// get current speed and turn based on offsets
	currTurn = 0.f; currSpeed = 0.f;

	// Both pressed simutaneously, move with the one last pressed
	if (forward && backward) {
		currSpeed = lastMove ? RUN_OFFSET : -RUN_OFFSET;
	}

	// Only one pressed
	else {
		if (forward)  { 
			currSpeed = RUN_OFFSET;  
		}

		if (backward) {
			currSpeed = -RUN_OFFSET;
		}
	}

	// Both pressed simutaneously, turn with the one last pressed
	if (left && right) {
		currTurn = lastTurn ? TURN_OFFSET : -TURN_OFFSET;
	}
	
	// Only one pressed
	else {
		if (left) {
			currTurn = -TURN_OFFSET;
		}

		if (right) {
			currTurn = TURN_OFFSET;
		}
	}

	// get current and last time to get delta
	lastTime = currTime;
	currTime = glfwGetTime();

	// not moving, set curr model to be idle
	if (currSpeed == 0.f && currTurn == 0.f) {
		current = idle;
	}

	// apply moving transformation, set curr model to walking
	else {
		current = walking;
		move();
	}
}

void Player::move() {
	float delta = (float)currTime - lastTime;
	rotate.y += currTurn * delta;

	float distance = currSpeed * delta;
	translate.x += (float)(distance * glm::sin(rotate.y));
	translate.z += (float)(distance * glm::cos(rotate.y));
}

void Player::draw(glm::mat4 view, glm::mat4 projection, GLuint shader) {
	glm::mat4 parent = GetTranslation() * GetRotation() * GetScale();
	current->draw(view, projection, parent, shader);
}

void Player::Forward() {
	forward = true;
	lastMove = true;
	// currSpeed = RUN_OFFSET;
}

void Player::Backward() {
	backward = true;
	lastMove = false;
	//currSpeed = -RUN_OFFSET;
}

void Player::Right() {
	right = true;
	lastTurn = true;
	//currTurn = TURN_OFFSET;
}

void Player::Left() {
	left = true;
	lastTurn = false;
	//currTurn = -TURN_OFFSET;
}
glm::mat4 Player::GetRotation() {
	/*
	return glm::rotate(rotate.z, glm::vec3(0.0f, 0.0f, 1.0f)) 
		* glm::rotate(rotate.y, glm::vec3(0.0f, 1.0f, 0.0f)) 
		* glm::rotate(rotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
	*/
	return glm::rotate(rotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
}
glm::mat4 Player::GetTranslation() {
	return glm::translate(translate);
}

glm::mat4 Player::GetScale() {
	return glm::translate(scale);
}

glm::vec3 Player::GetPosition() const
{
	return translate;
}

void Player::StopMovingForward() {
	forward = false;
}

void Player::StopMovingBackward() {
	backward = false;
}

void Player::StopTurningLeft() {
	left = false;
}

void Player::StopTurningRight() {
	right = false;
}