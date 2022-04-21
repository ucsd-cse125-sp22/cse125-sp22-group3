#include "Player.h"
#include "GameManager.h"

Player::Player() {
	translate = new glm::vec2(0.f,0.f);
	rotate = glm::vec3(0.0f);
	scale = glm::vec3(0.0f);

	collider_ = new ColliderCircle(glm::vec2(0,0), 3, false);
}

Player::Player(Model curr) : Player() {
	// Set initial values
	model = curr;
	std::cout << model.getBoneCount() << std::endl;
}

void Player::FixedUpdate() {
	if (glm::length(move_input) > 1) move_input = glm::normalize(move_input); 
	const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
	// If no movement is given apply friction (epsilon to account for FP errors)
	if (glm::length(move_input) < glm::epsilon<float>()) {
		if (glm::length(curr_vel_) < friction_ * delta) curr_vel_ = glm::vec2(0,0);
		else {
			curr_vel_ -= glm::normalize(curr_vel_) * friction_ * delta;
		}
		model.setAnimationMode(IDLE);

	}
	else {
		// Accelerate in our inputted direction
		// Transform input from 2D to 3D Plane
		curr_vel_ += delta * base_accel_ * move_input;
		// Cap our speed at some max velocity
		if (glm::length(curr_vel_) > max_velocity_ * glm::length(move_input)) {
			curr_vel_ = glm::normalize(curr_vel_) * max_velocity_ * glm::length(move_input);
		}
		
		model.setAnimationMode(WALK);
	}
	
	if (glm::length(curr_vel_) > 0) Move();

	collider_->center = *translate;
}


void Player::Move() {
	const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
	const glm::vec2 distance = delta * curr_vel_;
	*translate += distance;
	
	rotate.y = atan2(curr_vel_.x, -curr_vel_.y); //TODO fix this potentially
}

void Player::Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) {
	glm::mat4 parent = GetTranslation() * GetRotation() * GetScale();
	model.draw(view, projection, parent, shader);
}

void Player::Draw(GLuint shader) {
	glm::mat4 parent = GetTranslation() * GetRotation() * GetScale();
	model.draw(parent, shader);
}

std::vector<Collider*> Player::GetColliders()
{
	return { collider_ };
}

glm::vec2* Player::GetWorldPosition()
{
	return translate;
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
	return glm::translate(glm::vec3((* translate)[0], 0, -(*translate)[1]));
}

glm::mat4 Player::GetScale() {
	return glm::translate(scale);
}

glm::vec3 Player::GetPosition() const
{
	return glm::vec3((*translate)[0], 0, -(*translate)[1]);
}

void Player::SetPosition(const glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
}

void Player::SetTriggeringEntity(GameEntity* entity)
{
	entityTriggered = entity;
}

GameEntity* Player::GetTriggeringEntity()
{
	return entityTriggered;
}

void Player::SetHoldEntity(GameEntity* entity)
{
	isHolding = true;
	entityHeld = entity;
	// Switch models here?
}
