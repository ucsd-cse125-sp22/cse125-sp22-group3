#include "Indicator.h"
#include "Window.h"

Indicator::Indicator(VegetableType veggie) {
	type = veggie;
	if (veggie_map[type].requires_water) {
		model = INDICATOR_WATER;
	}
	else if (veggie_map[type].requires_fertilizer) {
		model = INDICATOR_FERTILIZER;
	}
	else {
		model = INDICATOR_WATER;
		printf("why did you make an indicator class?\n");

	}
	translate = new glm::vec2(0.f, 0.f);
	translate3D = new glm::vec3(0.f, 0.f, 0.f);
	rotation = glm::mat4(1);
}

Indicator::~Indicator() {}

void Indicator::FixedUpdate() {
	
	//TODO: get camera pos to set rotation
	//rotation = glm::lookAt(glm::normalize(glm::vec3((*translate3D)[0],currHeight, -(*translate3D)[2])), Window::eyePos, glm::vec3(0, 1, 0));
	//printf("hii eye: %f %f %f curr: %f f %f\n", Window::eyePos.x, Window::eyePos.y, Window::eyePos.z, (*translate3D)[0], currHeight, -(*translate3D)[2]);

	if (currHeight >= maxHeight) 
		goingUp = false;
	else if (currHeight <= minHeight)
		goingUp = true;
	if (goingUp)
		currHeight += heightIncrement;
	else
		currHeight -= heightIncrement;
}

glm::mat4 Indicator::GetParentTransform()
{
	return GetTranslation() * rotation;
}
ModelEnum Indicator::GetModelEnum() { return model; }

AniMode Indicator::GetAniMode() { return modelAnim; }

glm::vec3 Indicator::GetPosition() const
{
	return glm::vec3((*translate)[0], currHeight, -(*translate)[1]);
}

glm::mat4 Indicator::GetTranslation() {
	return glm::translate(glm::vec3((*translate3D)[0], currHeight, -(*translate3D)[2]));
}

glm::mat4 Indicator::GetRotation() const
{
	return rotation;
}

void Indicator::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
	*translate3D = glm::vec3(position[0], currHeight, -position[2]);
}

void Indicator::SetRotation(glm::mat4 rotation)
{
	this->rotation = rotation;
}

glm::mat4 Indicator::GetTransformation() {
	return glm::translate(glm::vec3((*translate3D)[0], currHeight, -(*translate3D)[2])) * rotation;
}

void Indicator::SetWater() {
	model = INDICATOR_WATER;
}

void Indicator::SetFertilize() {
	model = INDICATOR_FERTILIZER;
}