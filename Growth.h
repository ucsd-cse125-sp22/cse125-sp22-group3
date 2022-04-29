#pragma once
#include "Plot.h"

class Growth : public Plot {
public:
	Growth(ModelEnum curr);
	~Growth();

private:
	// Transformations
	glm::vec2* translate = nullptr;
	glm::mat4 rotation{};

	// Current model to display
	ModelEnum growthStage;
};