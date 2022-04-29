#include "Plot.h"

Plot::Plot(ModelEnum curr) {
	model = curr;

	translate = new glm::vec2(0.f, 0.f);
	rotation = glm::mat4(1);
	
	// Change to AABB later
	collider_ = new ColliderCircle(glm::vec2(0, 0), 5, false);
	collider_->collider_is_trigger = true;
}

Plot::~Plot() {}
Plot::Plot() {}
void Plot::FixedUpdate() {
	collider_->center = *translate;
}

void Plot::Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) {
	glm::mat4 parent = GetTransformation();
	//model.draw(view, projection, parent, shader);
}

void Plot::Draw(GLuint shader) {
	glm::mat4 parent = GetTransformation();
}

glm::mat4 Plot::GetTransformation() {
	const glm::mat4 trans = glm::translate(glm::vec3((*translate)[0], 0, -(*translate)[1])) * rotation;
	return trans;
}

std::vector<Collider*> Plot::GetColliders() {
	return { collider_ };
}

glm::vec2* Plot::GetWorldPosition() {
	return translate;
}

glm::vec3 Plot::GetPosition() const {
	return glm::vec3((*translate)[0], 0, -(*translate)[1]);
}

ModelEnum Plot::GetModel()
{
	return model;
}

glm::mat4 Plot::GetParentTransform()
{
	return GetTransformation() * rotation;
}

void Plot::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
}

void Plot::SetRotation(glm::mat4 rotation) {
	this->rotation = rotation;
}

bool Plot::CanInteract(Player* player) {
	return player->GetIsHolding();
	//return true;
}

void Plot::OnInteract(Player* player) {}

void Plot::SetPlantedVegetable(Seed* seed) {
	plantedVegetable = seed;
}

Vegetable* Plot::GetPlantedVegetable() {
	//return plantedVegetable;
	//TODO DONT FORGET
	return nullptr;
}

glm::vec2 Plot::GetTranslate()
{
	return *translate;
}
