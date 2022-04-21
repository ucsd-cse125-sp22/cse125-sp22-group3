#pragma once

#include "glm.h"

class Holdable
{
public:
    virtual glm::mat4 GetHoldTransform() = 0;
    virtual void OnDrop() = 0;
};