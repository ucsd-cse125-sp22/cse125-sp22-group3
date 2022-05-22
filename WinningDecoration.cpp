#include "WinningDecoration.h"

WinningDecoration::WinningDecoration(DecorationType type, glm::vec3 podiumPosition, std::default_random_engine* gen) {
	decType = type;
	podiumPos = podiumPosition;
	generator = gen;

	translate = new glm::vec2(0.f, 0.f);
	translate3D = new glm::vec3(0.f, 0.f, 0.f);
	rotation = glm::mat4(1);

	if (decType == PODIUM)
	{
		model = WORLD_PODIUM;
		SetPosition(podiumPosition);
	}
	else
	{
		RegenBalloonProperties();
	} 
}

WinningDecoration::~WinningDecoration() {
	delete translate;
	delete translate3D;
}

void WinningDecoration::RegenBalloonProperties()
{
	std::uniform_int_distribution<int> color_dist{ ModelEnum::BALLOON_YELLOW, ModelEnum::BALLOON_GREEN};
	std::uniform_real_distribution<float> velocity_dist{0.05f, 0.5f};
	std::uniform_real_distribution<float> pos_x_dist{-50.f + podiumPos[0], 50.f + podiumPos[0] };
	std::uniform_real_distribution<float> pos_y_dist{-2, 20};
	std::uniform_real_distribution<float> pos_z_dist{-50.f + podiumPos[2], 50.f + podiumPos[2] };

	this->model = static_cast<ModelEnum>(color_dist(*generator));
	this->upVelocity = velocity_dist(*generator);
	
gen_x_pos:
	float x = pos_x_dist(*generator);
	if (x >= podiumPos[0] - 8 && x <= podiumPos[0] + 8)
	{
		goto gen_x_pos;
	}
	
	float y = pos_y_dist(*generator);

gen_z_pos:
	float z = pos_z_dist(*generator);
	if (z >= podiumPos[2] - 8 && z <= podiumPos[2] + 8)
	{
		goto gen_x_pos;
	}
	this->SetPosition(glm::vec3(x, y, z));
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
		currHeight = -4;
		RegenBalloonProperties();
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
