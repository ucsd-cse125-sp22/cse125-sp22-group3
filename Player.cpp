#include "Player.h"

#include "GameManager.h"

Player::Player() {
	translate = glm::vec3(0.0f);
	rotate = glm::vec3(0.0f);
	scale = glm::vec3(0.0f);
}

Player::Player(Model * curr) {
	// Set initial values
	current = curr;
	idle = curr;

	translate = glm::vec3(0.0f);
	rotate = glm::vec3(0.0f);
	scale = glm::vec3(0.0f);
}

void Player::addIdle(Model* i) {
	idle = i;
}

void Player::addWalking(Model* w) {
	walking = w;
	walking->anim_speed = 2;
}

void Player::FixedUpdate() {
	const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
	// If no movement is given apply friction (epsilon to account for FP errors)
	if (glm::length(move_input) < glm::epsilon<float>()) {
		if (glm::length(curr_vel_) < friction_ * delta) curr_vel_ = glm::vec3(0,0,0);
		else {
			curr_vel_ -= glm::normalize(curr_vel_) * friction_ * delta;
		}
		current = idle;

	}
	else {
		// Accelerate in our inputted direction
		// Transform input from 2D to 3D Plane
		curr_vel_ += delta * base_accel_ * glm::vec3(move_input[0], 0, -move_input[1]);
		// Cap our speed at some max velocity
		if (glm::length(curr_vel_) > max_velocity_) curr_vel_ = glm::normalize(curr_vel_) * max_velocity_;
		
		current = walking;
	}
	
	if (glm::length(curr_vel_) > 0) Move();
}

float Lerp(const float a, const float b, const float f) //TODO move to a more global scope
{
	return a + f * (b - a);
}


void Player::Move() {
	const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
	const glm::vec3 distance = delta * curr_vel_;
	translate += distance;
	
	rotate.y = atan2(curr_vel_.x, curr_vel_.z);
}

void Player::Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) {
	glm::mat4 parent = GetTranslation() * GetRotation() * GetScale();
	current->draw(view, projection, parent, shader);
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