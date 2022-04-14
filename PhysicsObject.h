#pragma once
#include <glm/vec2.hpp>

#include "Collider.h"

class PhysicsObject
{
public:
    virtual void OnCollide() = 0;
    virtual Collider* GetCollider() = 0;
    virtual glm::vec2* GetWorldPosition() = 0;
};

