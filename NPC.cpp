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
	collider_->center = *translate;
}

glm::mat4 NPC::GetParentTransform()
{
	const glm::mat4 trans = glm::translate(glm::vec3((*translate)[0], 0, -(*translate)[1])) * rotation;
	return trans * rotation;
}

ModelEnum NPC::GetModelEnum()
{
	return model;
}

AniMode NPC::GetAniMode()
{
	return modelAnim;
}

std::vector<Collider*> NPC::GetColliders()
{
	return { collider_ };
}

glm::vec2* NPC::GetWorldPosition()
{
	return translate;
}

glm::vec3 NPC::GetPosition() const
{
	return glm::vec3((*translate)[0], 0, -(*translate)[1]);
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
	printf("Interacting with NPC\n");
	// Enable Shop UI
	player->ui_open = true;
	// Disable movement of player
}

glm::mat4 NPC::GetRotation()
{
	return glm::mat4();
}