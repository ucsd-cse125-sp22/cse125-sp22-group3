#include "World.h"

World::World(ModelEnum curr) {
	model = curr;
	modelAnim = NO_ANI;

	// collider should probably be aabb instead of circle erm
	collider_ = new ColliderCircle(glm::vec2(0, 0), 5, true);
	collider_->collider_is_trigger = true;

	translate = new glm::vec2(0.f, 0.f);
}

World::~World() {}

void World::FixedUpdate() {}

glm::mat4 World::GetParentTransform()
{
	return GetTranslation();
}

ModelEnum World::GetModelEnum() { return model; }
AniMode World::GetAniMode() { return modelAnim; }

void World::SetModel(ModelEnum newModel, glm::vec2 pos)
{
	model = newModel;
	*translate = pos;
}

std::vector<Collider*> World::GetColliders()
{
	return { collider_ };
}

glm::vec2* World::GetWorldPosition()
{
	return translate;
}

glm::vec3 World::GetPosition() const
{
	return glm::vec3((*translate)[0], 0, -(*translate)[1]);
}

void World::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
}

glm::mat4 World::GetTranslation() {
	return glm::translate(glm::vec3((*translate)[0], -(*translate)[1], 0));
}
