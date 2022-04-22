#include "Vegetable.h"

#include <stdbool.h>

Vegetable:: Vegetable(VegetableType vegetable, ModelEnum curr) {
	model = curr;

	type = vegetable;

	translate = new glm::vec2(0.f, 0.f);
	rotation = glm::mat4(1);

	collider_ = new ColliderCircle(glm::vec2(0, 0), 3, false);
	collider_->collider_is_trigger = true;

	hold_transformation_ = glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));
}

Vegetable::~Vegetable() {}

void Vegetable::FixedUpdate()
{
	collider_->center = *translate;
}

void Vegetable::Draw(glm::mat4 view, glm::mat4 projection, GLuint shader)
{
	glm::mat4 parent = GetTransformation();
	//model.draw(view, projection, parent, shader); TODO remove Draw function from Drawable
}

void Vegetable::Draw(GLuint shader) {
	glm::mat4 parent = GetTranslation();
	model.draw(parent, shader);
}

std::vector<Collider*> Vegetable::GetColliders()
{
	return { collider_ };
}

glm::vec2* Vegetable::GetWorldPosition()
{
	return translate;
}

glm::vec3 Vegetable::GetPosition() const
{
	return glm::vec3((*translate)[0], 0, -(*translate)[1]);
}

void Vegetable::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
}

void Vegetable::SetRotation(glm::mat4 rotation)
{
	this->rotation = rotation;
}

bool Vegetable::CanInteract(Player* player) { return !player->GetIsHolding(); }

void Vegetable::OnInteract(Player* player) { isHeld = true;}
void Vegetable::OnDrop() { isHeld = false; }
glm::mat4 Vegetable::GetHoldTransform() { return hold_transformation_; }

glm::mat4 Vegetable::GetTransformation()
{
	const glm::mat4 trans = glm::translate(glm::vec3((*translate)[0], 0, -(*translate)[1])) * rotation;
	return isHeld ? trans * hold_transformation_ : trans;
}
