#include "Seed.h"
#include "GameManager.h"

Seed::Seed(VegetableType vegetable, ModelEnum curr) {
	type = vegetable;
	modelAnim = NO_ANI;
	model = curr;

	translate = new glm::vec2(0.f, 0.f);
	rotation = glm::mat4(1);

	collider_ = new ColliderCircle(glm::vec2(0, 0), 5, false);
	collider_->collider_is_trigger = true;

	hold_transformation_ = glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));
}

Seed::~Seed() {}


void Seed::FixedUpdate()
{
	if (isPlanted) {
		plantedTime += GameManager::GetFixedDeltaTime();
		if (getIsReady()) {
			model = ModelEnum::VEG_CARROT;
		}
	}
	collider_->center = *translate;
}

glm::mat4 Seed::GetParentTransform()
{
	return GetTranslation() * rotation;
}
ModelEnum Seed::GetModelEnum() { return model; }
AniMode Seed::GetAniMode() { return modelAnim; }

void Seed::SetModel(ModelEnum newModel, glm::vec2 pos)
{
	model = newModel;
	*translate = pos;
	// rotate 90 degrees towards viewer
	rotation = glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(0,1,0));
	//setModelChanged(true);
}

std::vector<Collider*> Seed::GetColliders()
{
	return { collider_ };
}

glm::vec2* Seed::GetWorldPosition()
{
	return translate;
}

glm::vec3 Seed::GetPosition() const
{
	return glm::vec3((*translate)[0], 0, -(*translate)[1]);
}

void Seed::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
}

void Seed::SetRotation(glm::mat4 rotation)
{
	this->rotation = rotation;
}

glm::mat4 Seed::GetTranslation() {
	return glm::translate(glm::vec3((*translate)[0], 0, -(*translate)[1]));
}

glm::mat4 Seed::GetRotation() const
{
	return rotation;
}

VegetableType Seed::GetType()
{
	return type;
}

bool Seed::getIsReady()
{
	switch (type) {
	case VegetableType::TOMATO:
		if (plantedTime >= 5)
			return true;
		break;
	case VegetableType::CORN:
		if (plantedTime >= 10)
			return true;
		break;
	case VegetableType::CARROT:
		if (plantedTime >= 15)
			return true;
		break;
	case VegetableType::RADISH:
		if (plantedTime >= 20)
			return true;
		break;
	case VegetableType::CABBAGE:
		if (plantedTime >= 25)
			return true;
		break;

	}
	return false;
}

bool Seed::CanInteract(Player* player) { 
	return !player->GetIsHolding() && !isPlanted; 
}

void Seed::OnInteract(Player* player) { 
	isHeld = true; 
}

void Seed::OnDrop() { 
	isHeld = false; 
}

glm::mat4 Seed::GetHoldTransform() { 
	return hold_transformation_; 
}

void Seed::SetPlanted()
{
	isPlanted = true;
	plantedTime = 0;
}

glm::mat4 Seed::GetTransformation() {
	const glm::mat4 trans = glm::translate(glm::vec3((*translate)[0], 0, -(*translate)[1])) * rotation;
	return isHeld ? trans * hold_transformation_ : trans;
}

