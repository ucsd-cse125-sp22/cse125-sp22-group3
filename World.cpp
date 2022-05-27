#include "World.h"

World::World(ModelEnum curr) {
	model = curr;
	modelAnim = NO_ANI;

	// nicholas if u see this. i am soooo sorry for hardcoding sooo sorrie exclamation mark!
	colliders_.push_back(new ColliderCircle(glm::vec2(60.047, 64.737), 3, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(47.914, 102.282), 3, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(89.794, -72.309), 3, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(73.056, 115.183), 3, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(104.370, 90.524), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(93.236, 47.506), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(126.123, -14.300), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-60.702, 17.828), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-93.140, 10.614), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(73.125, -124.418), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-31.585, -57.759), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(39.382, -83.986), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-41.721, 76.790), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-129.842, 67.710), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-127.934, -27.875), 2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-9.432, -23.107), 1.5,true));
	colliders_.push_back(new ColliderCircle(glm::vec2(70.547, -40.986), 1.5,true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-128.331, 45.151), 1.5, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-113.925, 20.222), 1.5, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(52.618, -29.027), 1.5, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(116.341, 27.281), 1.5, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(123.284, 51.372), 1.5, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(115.927, -27.333), 1.5, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-17.764, -42.188), 1.5, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(126.895, -131.359), 3, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(115.120, -90.907), 1.2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(105.972, -100.859), 1.2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(23.812, 125.393), 1.2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(45.301, 132.372), 1.2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-14.141, 126.720), 1.2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-58.899, 123.030), 1.2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-60.256, 101.020), 1.2, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-1.013875, 44.297501), 1.2, true));

	colliders_.push_back(new ColliderCircle(glm::vec2(-121.937, -114.008), 12, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(92.839, -123.126), 12, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(114.126, 123.026), 12, true));
	colliders_.push_back(new ColliderCircle(glm::vec2(-128.408, 123.026), 12, true));

	colliders_.push_back(new ColliderAABB(glm::vec2(-140, -135), glm::vec2(-135, 135), true));
	colliders_.push_back(new ColliderAABB(glm::vec2(135, -135), glm::vec2(140, 135), true));
	colliders_.push_back(new ColliderAABB(glm::vec2(-135, -140), glm::vec2(135, -135), true));
	colliders_.push_back(new ColliderAABB(glm::vec2(-135, 135), glm::vec2(135, 140), true));
	
	// NPC Stand
	colliders_.push_back(new ColliderAABB(glm::vec2(-5.628884, 3.699146), glm::vec2(4.809305, 8.521981), true));

	// NPC
	colliders_.push_back(new ColliderCircle(glm::vec2(0.475988, 0.107014), 1.5, true));

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
	return colliders_;
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
