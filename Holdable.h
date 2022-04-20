#pragma once
#include <glm/fwd.hpp>

class Holdable
{
public:
    virtual glm::mat4 GetHoldTransform() = 0;
    virtual void OnDrop() = 0;
};