#include "WinningDecoration.h"

WinningDecoration::WinningDecoration(ModelEnum curr) {
	model = curr;
	modelAnim = NO_ANI;

	translate = new glm::vec2(0.f, 0.f);
	translate3D = new glm::vec3(0.f, 0.f, 0.f);
	rotation = glm::mat4(1);
}

WinningDecoration::~WinningDecoration() {
	delete translate;
	delete translate3D;
}

ModelEnum WinningDecoration::GetModelEnum() { return model; }

AniMode WinningDecoration::GetAniMode() { return modelAnim; }

glm::mat4 WinningDecoration::GetParentTransform()
{
	return GetTranslation() * rotation;
}

void WinningDecoration::FixedUpdate()
{
	currHeight += upVelocity;

	if (currHeight > maxHeight) {
		currHeight = 0;
	}
}

glm::vec3 WinningDecoration::GetPosition() const
{
	return glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2]);
}

void WinningDecoration::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
	*translate3D = glm::vec3(position[0], position[1], -position[2]);
}

glm::mat4 WinningDecoration::GetRotation() const
{
	return rotation;
}

void WinningDecoration::SetRotation(glm::mat4 rotation)
{
	this->rotation = rotation;
}

float WinningDecoration::GetHeight()
{
	return (*translate3D)[1];
}

void WinningDecoration::SetHeight(float height)
{
	(*translate3D)[1] = height;
}

glm::mat4 WinningDecoration::GetTransformation()
{
	return glm::translate(glm::vec3((*translate)[0], currHeight, -(*translate)[1])) * rotation;
}

glm::mat4 WinningDecoration::GetTranslation() {
	return glm::translate(glm::vec3((*translate3D)[0], currHeight, -(*translate3D)[2]));
}
