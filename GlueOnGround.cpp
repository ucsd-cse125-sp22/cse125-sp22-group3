#include "GlueOnGround.h"

GlueOnGround::GlueOnGround() {
	translate = new glm::vec2(0.f, 0.f);
	translate3D = new glm::vec3(0.f, 0.f, 0.f);
	rotation = glm::mat4(1);

	collider_ = new ColliderCircle(glm::vec2(0, 0), 5, false);
	collider_->collider_is_trigger = true;
}

GlueOnGround::~GlueOnGround() {
	delete collider_;
	delete translate;
	delete translate3D;
}


void GlueOnGround::FixedUpdate()
{
	collider_->center = *translate;
	if (gluedPlayer != nullptr) {
		const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
		glueTime += delta;
	}
	if (glueTime >= maxGlueTime) {
		gluedPlayer->isGlued = false;
		GameManager::RemoveEntities({ this });
	}
}

void GlueOnGround::OnCollide(PhysicsObject *object) {
	if (auto player = dynamic_cast<Player*>(object)) {
		if (ownerOfGlue == player || gluedPlayer !=nullptr)
			return;
		else {
		
			gluedPlayer = player;
			player->isGlued = true;

			glueTime = 0;
		}
	}
}

glm::mat4 GlueOnGround::GetParentTransform()
{
	return GetTranslation() * rotation;
}

ModelEnum GlueOnGround::GetModelEnum() { return model; }
AniMode GlueOnGround::GetAniMode() { return modelAnim; }

std::vector<Collider*> GlueOnGround::GetColliders()
{
	return { collider_ };
}

glm::vec2* GlueOnGround::GetWorldPosition()
{
	return translate;
}

glm::vec3 GlueOnGround::GetPosition() const
{
	return glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2]);
}

void GlueOnGround::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
	*translate3D = glm::vec3(position[0], position[1], -position[2]);
}

void GlueOnGround::SetRotation(glm::mat4 rotation)
{
	this->rotation = rotation;
}

glm::mat4 GlueOnGround::GetTranslation() {
	return glm::translate(glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2]));
}

glm::mat4 GlueOnGround::GetRotation() const
{
	return rotation;
}

glm::mat4 GlueOnGround::GetTransformation() {
	const glm::mat4 trans = glm::translate(glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2])) * rotation;
	return trans;
}
