#include "Particle.h"

Particle::Particle(ModelEnum curr) {
	model = curr;

	translate = new glm::vec2(0.f, 0.f);
	rotation = glm::mat4(1); // TODO calculate rotation based on angle the camera is tilted
}

Particle::~Particle() {}

void Particle::FixedUpdate() {}

glm::mat4 Particle::GetParentTransform()
{
	return GetTranslation() * rotation;
}
ModelEnum Particle::GetModelEnum() { return model; }

AniMode Particle::GetAniMode() { return modelAnim; }

glm::vec3 Particle::GetPosition() const
{
	return glm::vec3((*translate)[0], 0, -(*translate)[1]);
}

glm::mat4 Particle::GetTranslation() {
	return glm::translate(glm::vec3((*translate)[0], 0, -(*translate)[1]));
}

glm::mat4 Particle::GetRotation() const
{
	return rotation;
}

void Particle::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
}

void Particle::SetRotation(glm::mat4 rotation)
{
	this->rotation = rotation;
}