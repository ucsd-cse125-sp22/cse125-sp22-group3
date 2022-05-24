#include "Particle.h"

Particle::Particle(ModelEnum curr) {
	model = curr;

	translate = new glm::vec2(0.f, 0.f);
	translate3D = new glm::vec3(0.f, 0.f, 0.f);
	rotation = glm::rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
}

Particle::~Particle()
{
	delete translate;
	delete translate3D;
}

void Particle::FixedUpdate() {}

glm::mat4 Particle::GetParentTransform()
{
	return GetTranslation() * rotation;
}
ModelEnum Particle::GetModelEnum() { return model; }

AniMode Particle::GetAniMode() { return modelAnim; }

glm::vec3 Particle::GetPosition() const
{
	return glm::vec3((*translate)[0], (*translate3D)[1], -(*translate)[1]);
}

glm::mat4 Particle::GetTranslation() {
	return glm::translate(glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2]));
}

glm::mat4 Particle::GetRotation() const
{
	return rotation;
}

void Particle::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
	*translate3D = glm::vec3(position[0], position[1], -position[2]);
}

void Particle::SetRotation(glm::mat4 rotation)
{
	this->rotation = rotation;
}

glm::mat4 Particle::GetTransformation() {
	return glm::translate(glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2])) * rotation;
}