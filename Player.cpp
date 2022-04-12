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
	walking->anim_speed = 2;
}

void Player::update() {
	auto move_dir = glm::vec3(0,0,0);
	if (forward) move_dir += glm::vec3(0,0,-1);
	if (backward) move_dir += glm::vec3(0,0,1);
	if (right) move_dir += glm::vec3(1,0,0);
	if (left) move_dir += glm::vec3(-1,0,0);

	// If no movement is given apply friction (epsilon to account for FP errors)
	if (glm::length(move_dir) < glm::epsilon<float>()) {
		if (glm::length(curr_vel_) < friction_) curr_vel_ = glm::vec3(0,0,0);
		else {
			curr_vel_ -= (glm::normalize(curr_vel_) * friction_);
		}
		current = idle;

	}
	else {
		// Accelerate in our inputted direction
		curr_vel_ += base_accel_ * move_dir;
		// Cap our speed at some max velocity
		if (glm::length(curr_vel_) > max_velocity_) curr_vel_ = glm::normalize(curr_vel_) * max_velocity_;
		
		current = walking;
	}
	
	// get current and last time to get delta
	lastTime = currTime;
	currTime = glfwGetTime();

	if (glm::length(curr_vel_) > 0) move();
}

float Lerp(const float a, const float b, const float f) //TODO move to a more global scope
{
	return a + f * (b - a);
}


void Player::move() {
	const float delta = (float)currTime - lastTime; //TODO this should be more global
	const glm::vec3 distance = delta * curr_vel_;
	translate += distance;
	
	rotate.y = atan2(curr_vel_.x, curr_vel_.z);
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