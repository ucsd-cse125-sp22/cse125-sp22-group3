#include "Growth.h"

Growth::Growth(ModelEnum curr) {
	growthStage = curr;

	translate = new glm::vec2(0.f, 0.f);
	rotation = glm::mat4(1);

}

Growth::~Growth() {}


