#pragma once
#include <glm/fwd.hpp>

class Holdable
{
    virtual glm::mat4 GetHoldTransform() = 0;
};