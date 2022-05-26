#include "NPC.h"

NPC::NPC(ModelEnum curr)
{
	model = curr;

	translate = new glm::vec2(0.f, 0.f);
	rotation = glm::mat4(1);

	// Change to AABB later
	collider_ = new ColliderCircle(glm::vec2(0, 0), 6, false);
	collider_->collider_is_trigger = true;
}

NPC::~NPC()
{
	delete translate;
	delete collider_;
}

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
	*translate = glm::vec2(position[0], -position[2]);
	//*translate3D = glm::vec3(position[0], position[1], -position[2]);
}

bool NPC::CanInteract(Player* player)
{
	//TODO: if bribed return false
	return true;
}

void NPC::OnInteract(Player* player)
{
	if (player->GetIsHolding()) {
		// TODO: instead of calling sell directly, open sell GUI instead
		player->OpenSaleUI(); 
		//player->Sell();
	}
	else {
		// Enable Shop UI and disable movement
		player->OpenUI();

	}
	
}

glm::mat4 NPC::GetRotation()
{
	return glm::mat4();
}