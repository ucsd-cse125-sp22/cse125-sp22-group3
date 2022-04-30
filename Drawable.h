#pragma once

#include "glm.h"
#include "util.h"

class Drawable {
	public:
		virtual ModelEnum GetModelEnum() = 0;
		virtual AniMode GetAniMode() = 0;
		virtual glm::mat4 GetParentTransform() = 0;
};