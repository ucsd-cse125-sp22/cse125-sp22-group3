#include "Vegetable.h"

#include <stdbool.h>

Vegetable::Vegetable(VegetableType vegetable, ModelEnum curr) {
	model = curr;
	type = vegetable;
	modelAnim = NO_ANI;

	translate = new glm::vec2(0.f, 0.f);
	translate3D = new glm::vec3(0.f, 0.f,0.f);
	rotation = glm::mat4(1);

	collider_ = new ColliderCircle(glm::vec2(0, 0), 3, false);
	collider_->collider_is_trigger = true;

	hold_transformation_ = glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));

	// rand location for golden eggplant
	if (vegetable == VegetableType::GOLDEN_EGGPLANT) {
		float mapMin = -135;
		float mapMax = 135;
		float randX= mapMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (mapMax - mapMin)));
		float randZ = mapMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (mapMax - mapMin)));
		*translate = glm::vec2(randX, randZ);
		*translate3D = glm::vec3(randX, 0.f, randZ);
		printf("GOLDEN EGGPLANT HAS SPAWNED AT %f %f\n", randX, randZ);
	}
}

Vegetable::~Vegetable() {
	delete collider_;
	delete translate;
	delete translate3D;
}

void Vegetable::FixedUpdate()
{
	collider_->center = *translate;
}

glm::mat4 Vegetable::GetParentTransform()
{
	return GetTranslation() * rotation;
}
ModelEnum Vegetable::GetModelEnum() { return model; }
AniMode Vegetable::GetAniMode() { return modelAnim; }

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
	return glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2]);
}

glm::mat4 Vegetable::GetTranslation() {
	return glm::translate(glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2]));
}

glm::mat4 Vegetable::GetRotation() const
{
	return rotation;
}

void Vegetable::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
	*translate3D = glm::vec3(position[0],position[1], - position[2]);
}

void Vegetable::SetRotation(glm::mat4 rotation)
{
	this->rotation = rotation;
}

bool Vegetable::CanInteract(Player* player) { return !player->GetIsHolding(); }

void Vegetable::OnInteract(Player* player)
{
	if (holding_player != nullptr) {
		holding_player->Drop();
		player->sound_steal = true;
	}
	SetHeight(pickupHeight);
	player->SetHoldEntity(this);
	player->SetTriggeringEntity(nullptr);

	isHeld = true;
	holding_player = player;
}
void Vegetable::OnDrop()
{
	SetHeight(dropHeight);
	isHeld = false;
	holding_player = nullptr;
}
glm::mat4 Vegetable::GetHoldTransform() { return hold_transformation_; }

void Vegetable::SetHeight(float height)
{
	(*translate3D)[1] = height;
}

glm::mat4 Vegetable::GetTransformation()
{
	const glm::mat4 trans = glm::translate(glm::vec3((*translate)[0], 0, -(*translate)[1])) * rotation;
	return isHeld ? trans * hold_transformation_ : trans;
}
float Vegetable::GetHeight()
{
	return (*translate3D)[1];
}
