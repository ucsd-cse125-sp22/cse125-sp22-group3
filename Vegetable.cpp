#include "Vegetable.h"

Vegetable:: Vegetable(VegetableType vegetable, Model curr) {
	model = curr;

	type = vegetable;

	translate = new glm::vec2(0.f, 0.f);

	collider_ = new ColliderCircle(glm::vec2(0, 0), 3, false);
	collider_->collider_is_trigger = true;
}

Vegetable::~Vegetable() {}

//implement virtual function of Interface Interactable
bool canInteract(Player* player) {
	return true; //TODO: return true by default 
}

void interact(Player* player) {}

void Vegetable::FixedUpdate()
{
	collider_->center = *translate;
}

void Vegetable::Draw(glm::mat4 view, glm::mat4 projection, GLuint shader)
{
	glm::mat4 parent = GetTranslation();
	model.draw(view, projection, parent, shader);
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

bool Vegetable::canInteract(Player* player)
{
	return false;
}

void Vegetable::interact(Player* player)
{
}

glm::mat4 Vegetable::GetTranslation()
{
	return glm::translate(glm::vec3((*translate)[0], 0, -(*translate)[1]));

}
