#include "GardenTool.h"

GardenTool::GardenTool(ModelEnum curr) {
	modelAnim = NO_ANI;
	model = curr;

	translate = new glm::vec2(0.f, 0.f);
	translate3D = new glm::vec3(0.f, 0.f, 0.f);
	rotation = glm::mat4(1);

	collider_ = new ColliderCircle(glm::vec2(0, 0), 5, false);
	collider_->collider_is_trigger = true;
}

GardenTool::~GardenTool() {
	delete collider_;
	delete translate;
	delete translate3D;
}


void GardenTool::FixedUpdate()
{
	collider_->center = *translate;
}

glm::mat4 GardenTool::GetParentTransform()
{
	return GetTranslation() * rotation;
}

ModelEnum GardenTool::GetModelEnum() { return model; }
AniMode GardenTool::GetAniMode() { return modelAnim; }

void GardenTool::SetModel(ModelEnum newModel, glm::vec3 pos)
{
	model = newModel;
	*translate = glm::vec2(pos.x, pos.z);
	*translate3D = pos;
	//*translate3D = pos;
	// rotate 90 degrees towards viewer
	rotation = glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(0, 1, 0));
	//setModelChanged(true);
}

std::vector<Collider*> GardenTool::GetColliders()
{
	return { collider_ };
}

glm::vec2* GardenTool::GetWorldPosition()
{
	return translate;
}

glm::vec3 GardenTool::GetPosition() const
{
	return glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2]);
}

void GardenTool::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
	*translate3D = glm::vec3(position[0], position[1], -position[2]);
}

void GardenTool::SetRotation(glm::mat4 rotation)
{
	this->rotation = rotation;
}

glm::mat4 GardenTool::GetTranslation() {
	return glm::translate(glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2]));
}

glm::mat4 GardenTool::GetRotation() const
{
	return rotation;
}

void GardenTool::SetHeight(float height)
{
	(*translate3D)[1] = height;
}

float GardenTool::GetHeight()
{
	return (*translate3D)[1];
}

glm::mat4 GardenTool::GetTransformation() {
	const glm::mat4 trans = glm::translate(glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2])) * rotation;
	return isHeld ? trans * this->GetHoldTransform() : trans;
}
