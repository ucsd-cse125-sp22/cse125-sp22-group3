#include "Player.h"

#include "GameManager.h"

Player::Player() {
	translate = new glm::vec2(0.f,0.f);
	rotate = glm::vec3(0.0f);
	scale = glm::vec3(0.0f);

	collider_ = new ColliderCircle(glm::vec2(0,0), 3, false);
}

GameEntity* Player::GetHoldEntity()
{
	return entityHeld;
}

Player::Player(ModelEnum curr) : Player() {
	// Set initial values
	model = curr;
	//std::cout << model.getBoneCount() << std::endl;
}

void Player::FixedUpdate() {
	if (glm::length(move_input) > 1) move_input = glm::normalize(move_input); 
	const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
	// If no movement is given apply friction (epsilon to account for FP errors)
	if (glm::length(move_input) <= 0) {
		if (glm::length(curr_vel_) < friction_ * delta) curr_vel_ = glm::vec2(0,0);
		else {
			curr_vel_ -= glm::normalize(curr_vel_) * friction_ * delta;
		}

		if (isHolding) 
			this->modelAnim = IDLE_HOLD;
		else
			this->modelAnim = IDLE;
	}
	else {
		// Accelerate in our inputted direction
		// Transform input from 2D to 3D Plane
		curr_vel_ += base_accel_ * move_input * delta;
		// Cap our speed at some max velocity
		if (glm::length(curr_vel_) > max_velocity_ * glm::length(move_input)) {
			curr_vel_ = glm::normalize(curr_vel_) * max_velocity_ * glm::length(move_input);
		}
		
		if (isHolding)
			this->modelAnim = IDLE_WALK;
		else
			this->modelAnim = WALK;
	}
	
	if (glm::length(curr_vel_) > 0) {
		Move();
		MoveHeld();
	}

	collider_->center = *translate;
}


void Player::Move() {
	const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
	const glm::vec2 distance = delta * curr_vel_;
	*translate += distance;
	
	rotate.y = atan2(curr_vel_.x, -curr_vel_.y); //TODO fix this potentially
}


void Player::Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) {
	glm::mat4 parent = GetParentTransform();
	//model.draw(view, projection, parent, shader);
}

glm::mat4 Player::GetParentTransform()
{
	return GetTranslation() * GetRotation() * GetScale();
}

ModelEnum Player::GetModel()
{
	return model;
}

void Player::OnTrigger(PhysicsObject* object)
{
	auto entity = dynamic_cast<GameEntity*>(object);
	if (entity->type == EntityType::VEGETABLE) {
		auto veggie = dynamic_cast<Vegetable*>(object);
		if (veggie != nullptr && !isHolding) {
			if (entityTriggered == nullptr) {
				SetTriggeringEntity(veggie);
			}
			else {
				auto phys_entity = dynamic_cast<PhysicsObject*>(entityTriggered);
				const float dist = glm::length(*veggie->GetWorldPosition() - *GetWorldPosition());
				const float curr_dist = glm::length(*phys_entity->GetWorldPosition() - *GetWorldPosition());
				if (dist < curr_dist) {
					SetTriggeringEntity(veggie);
				}
			}
		}
	}
	else if (entity->type == EntityType::PLOT) {
		auto plot = dynamic_cast<Plot*>(object);
		if (plot != nullptr && isHolding) {
			if (entityTriggered == nullptr) {
				SetTriggeringEntity(plot);
			}
			else {
				auto phys_entity = dynamic_cast<PhysicsObject*>(entityTriggered);
				const float dist = glm::length(*plot ->GetWorldPosition() - *GetWorldPosition());
				const float curr_dist = glm::length(*phys_entity->GetWorldPosition() - *GetWorldPosition());
				if (dist < curr_dist) {
					SetTriggeringEntity(plot);
				}
			}
		}
	}

	else if (entity->type == EntityType::SEED) {
		auto seed = dynamic_cast<Seed*>(object);
		if (seed != nullptr && !isHolding) {
			if (entityTriggered == nullptr) {
				SetTriggeringEntity(seed);
			}
			else {
				auto phys_entity = dynamic_cast<PhysicsObject*>(entityTriggered);
				const float dist = glm::length(*seed->GetWorldPosition() - *GetWorldPosition());
				const float curr_dist = glm::length(*phys_entity->GetWorldPosition() - *GetWorldPosition());
				if (dist < curr_dist) {
					SetTriggeringEntity(seed);
				}
			}
		}
	}
}

void Player::Draw(GLuint shader) {
	glm::mat4 parent = GetTranslation() * GetRotation() * GetScale();
	//model.draw(parent, shader); TODO
}

std::vector<Collider*> Player::GetColliders()
{
	return { collider_ };
}

glm::vec2* Player::GetWorldPosition()
{
	return translate;
}
void Player::Use()
{	
	auto entity = dynamic_cast<PhysicsObject*>(entityTriggered);
	if (entity != nullptr && collider_->CollidesWith(entity->GetColliders()[0])) {
		auto interactable = dynamic_cast<Interactable*>(entityTriggered);
		if (interactable != nullptr && interactable->CanInteract(this)) {
			interactable->OnInteract(this);

			//VEGGIE SPECIFIC CODE
			//TODO Maybe change to holdable to make more general?
			if (entityTriggered->type == EntityType::VEGETABLE) {
				auto vegetable = dynamic_cast<Vegetable*>(entityTriggered);
				if (vegetable != nullptr && !isHolding) {
					SetHoldEntity(entityTriggered);
					SetTriggeringEntity(nullptr);
				}
			}
			else if (entityTriggered->type == EntityType::PLOT) {
				auto plot = dynamic_cast<Plot*>(entityTriggered);
				if (plot != nullptr) {
					if (isHolding && !plot->isPlanted) {
						printf("PLOT");
						auto vegetable = dynamic_cast<Vegetable*>(entityHeld);
						plot->isPlanted = true;
						plot->SetPlantedVegetable(vegetable);
						this->Drop();
						SetTriggeringEntity(nullptr);
					}
					else if (!isHolding && plot->isPlanted) {
						printf("UNPLOT");
						plot->isPlanted = false;
						SetTriggeringEntity(nullptr);
					}
				}
			}

			else if (entityTriggered->type == EntityType::SEED) {
				std::cout << "Here" << std::endl;
				auto seed = dynamic_cast<Seed*>(entityTriggered);
				if (seed != nullptr && !isHolding) {
					SetHoldEntity(entityTriggered);
					SetTriggeringEntity(nullptr);
				}
			}
		}
	}
}
void Player::Drop()
{
	auto holdable = dynamic_cast<Holdable*>(entityHeld);
	if (holdable != nullptr) {
		holdable->OnDrop();
	}
	isHolding = false;
	entityHeld = nullptr;
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

void Player::SetWorldPosition(const glm::vec3 position)
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
	MoveHeld();
	// Switch models here?
}

void Player::MoveHeld() {
	if (isHolding && entityHeld->type == EntityType::VEGETABLE) {
		auto vegetable = dynamic_cast<Vegetable*>(entityHeld);
		
		const glm::vec4 vegetableLocation = glm::vec4(0, 0, -entityHeldDist, 1) * GetRotation();
		const glm::mat4 held_rotation = glm::rotate(rotate.y, glm::vec3(0, 1, 0));
		vegetable->SetPosition(glm::vec3(translate->x + (vegetableLocation.x), 0, (-1) * translate->y - (vegetableLocation.z)));
		vegetable->SetRotation(held_rotation);
	}

	if (isHolding && entityHeld->type == EntityType::SEED) {
		auto seed = dynamic_cast<Seed*>(entityHeld);

		const glm::vec4 seedLocation = glm::vec4(0, 0, -entityHeldDist, 1) * GetRotation();
		const glm::mat4 held_rotation = glm::rotate(rotate.y, glm::vec3(0, 1, 0));
		seed->SetPosition(glm::vec3(translate->x + (seedLocation.x), 0, (-1) * translate->y - (seedLocation.z)));
		seed->SetRotation(held_rotation);
	}
}