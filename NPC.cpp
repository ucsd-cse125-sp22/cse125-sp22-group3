#include "NPC.h"

NPC::NPC(ModelEnum curr)
{
	model = curr;

	translate = new glm::vec2(0.f, 0.f);
	rotation = glm::mat4(1);

	// Change to AABB later
	collider_ = new ColliderCircle(glm::vec2(0, 0), 5, false);
	collider_->collider_is_trigger = true;
}

NPC::~NPC() {}

void NPC::FixedUpdate()
{
}

glm::mat4 NPC::GetParentTransform()
{
	return glm::mat4();
}

ModelEnum NPC::GetModelEnum()
{
	return ModelEnum();
}

AniMode NPC::GetAniMode()
{
	return AniMode();
}

std::vector<Collider*> NPC::GetColliders()
{
	return { collider_ };
}

glm::vec2* NPC::GetWorldPosition()
{
	return nullptr;
}

glm::vec3 NPC::GetPosition() const
{
	return glm::vec3();
}

void NPC::SetWorldPosition(glm::vec3 position)
{
}

bool NPC::CanInteract(Player* player)
{
	//TODO: if bribed return false
	return true;
}

void NPC::OnInteract(Player* player)
{
	// Enable Shop UI
	// Disable movement
}

glm::mat4 NPC::GetRotation()
{
	return glm::mat4();
}