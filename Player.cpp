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

glm::mat4 Player::GetParentTransform()
{
	return GetTranslation() * GetRotation() * GetScale();
}
ModelEnum Player::GetModelEnum() { return model; }
AniMode Player::GetAniMode() { return modelAnim; }

void Player::OnTrigger(PhysicsObject* object)
{
	auto entity = dynamic_cast<GameEntity*>(object);
	if (entity != nullptr) {
		if (entityTriggered == nullptr) {
			SetTriggeringEntity(entity);
		}
		else {
			const auto phys_entity = dynamic_cast<PhysicsObject*>(entityTriggered);
			const float dist = glm::length(*object->GetWorldPosition() - *GetWorldPosition());
			const float curr_dist = glm::length(*phys_entity->GetWorldPosition() - *GetWorldPosition());
			if (dist < curr_dist) {
				SetTriggeringEntity(entity);
			}
		}
	}
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
			auto temp = dynamic_cast<Plot*>(entityTriggered);
			auto temp1 = dynamic_cast<Vegetable*>(entityTriggered);
			if (auto vegetable = dynamic_cast<Vegetable*>(entityTriggered)){//auto vegetable = dynamic_cast<Vegetable*>(entityTriggered)) {
				if (!isHolding && (vegetable != nullptr)) {
					SetHoldEntity(vegetable);
					SetTriggeringEntity(nullptr);
				}
			}

			else if (auto plot = dynamic_cast<Plot*>(entityTriggered)) {
					if (isHolding && !plot->isPlanted) {	
						if (auto seed = dynamic_cast<Seed*>(entityHeld)) {

							plot->SetPlantedVegetable(seed);

							// maybe move this to a helper method its kinda bulky here
							switch (seed->GetType()) {
							case VegetableType::CABBAGE:
								seed->SetModel(ModelEnum::WORLD_FLAG_CABBAGE, plot->GetTranslate());
								break;
							case VegetableType::CORN:
								seed->SetModel(ModelEnum::WORLD_FLAG_CORN, plot->GetTranslate());
								break;
							case VegetableType::CARROT:
								seed->SetModel(ModelEnum::WORLD_FLAG_CARROT, plot->GetTranslate());
								break;
							case VegetableType::RADISH:
								seed->SetModel(ModelEnum::WORLD_FLAG_RADISH, plot->GetTranslate());
								break;
							case VegetableType::TOMATO:
								seed->SetModel(ModelEnum::WORLD_FLAG_TOMATO, plot->GetTranslate());
								break;

							default:
								printf("Error seed to flag not found\n");
							}
							seed->SetPlanted();
							this->Drop();
							SetTriggeringEntity(nullptr);
						}
						else {
							printf("Warning: You can only plant seeds not veggies bro\n");
						}
					}
					else if (!isHolding) {
						Seed* seed = plot->plantedVegetable;
						if (seed != nullptr && plot->plantedVegetable->isHarvestable) {
							// Remove the seed
							VegetableType veggieType = plot->GetPlantedVegetable();
							auto seed_ = dynamic_cast<GameEntity*>(seed);
							if (seed_ != nullptr) {
								GameManager::RemoveEntities({ seed_ });
								plot->SetPlantedVegetable(nullptr);
							}

							Vegetable* veggie = nullptr;
							// Spawn the correct vegetable on the player
							switch (seed->GetType()) {
							case VegetableType::CABBAGE:
								veggie = new Vegetable{ VegetableType::CABBAGE, VEG_CABBAGE };
								break;
							case VegetableType::CORN:
								veggie = new Vegetable{ VegetableType::CORN, VEG_CORN };
								break;
							case VegetableType::CARROT:
								veggie = new Vegetable{ VegetableType::CARROT, VEG_CARROT };
								break;
							case VegetableType::RADISH:
								veggie = new Vegetable{ VegetableType::RADISH, VEG_RADISH };
								break;
							case VegetableType::TOMATO:
								veggie = new Vegetable{ VegetableType::TOMATO, VEG_TOMATO };
								break;

							default:
								printf("Error flag to veggie not found\n");
							}

							GameManager::AddEntities({ veggie });
							SetHoldEntity(veggie);
							SetTriggeringEntity(nullptr);
						}
					}
					
			}
			else if (auto seed = dynamic_cast<Seed*>(entityTriggered)) {
				//std::cout << "Here" << std::endl;
				if (!isHolding) {
					SetHoldEntity(seed);
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
	if (isHolding) {
		if (auto vegetable = dynamic_cast<Vegetable*>(entityHeld)) {
		
			const glm::vec4 vegetableLocation = glm::vec4(0, 0, -entityHeldDist, 1) * GetRotation();
			const glm::mat4 held_rotation = glm::rotate(rotate.y, glm::vec3(0, 1, 0));
			vegetable->SetPosition(glm::vec3(translate->x + (vegetableLocation.x), 0, (-1) * translate->y - (vegetableLocation.z)));
			vegetable->SetRotation(held_rotation);
		}
		else if (auto seed = dynamic_cast<Seed*>(entityHeld)) {

			const glm::vec4 seedLocation = glm::vec4(0, 0, -entityHeldDist, 1) * GetRotation();
			const glm::mat4 held_rotation = glm::rotate(rotate.y, glm::vec3(0, 1, 0));
			seed->SetPosition(glm::vec3(translate->x + (seedLocation.x), 0, (-1) * translate->y - (seedLocation.z)));
			seed->SetRotation(held_rotation);
		}
	}

}