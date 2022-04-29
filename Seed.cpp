#include "Seed.h"

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
	collider_->center = *translate;
}

glm::mat4 Seed::GetParentTransform()
{
	return GetTranslation() * rotation;
}

ModelEnum Seed::GetModel()
{
	return model;
}

void Seed::SetModel(ModelEnum newModel, glm::vec2 pos)
{

	model = newModel;
	*translate = pos;
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

bool Seed::CanInteract(Player* player) { 
	return !player->GetIsHolding(); 
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

glm::mat4 Seed::GetTransformation() {
	const glm::mat4 trans = glm::translate(glm::vec3((*translate)[0], 0, -(*translate)[1])) * rotation;
	return isHeld ? trans * hold_transformation_ : trans;
}

