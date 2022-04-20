#pragma once
#include <glm/vec2.hpp>

#include "Collider.h"
#include <vector>

class PhysicsObject
{
public:
    virtual void OnCollide(PhysicsObject* object) = 0;
    virtual void OnTrigger(PhysicsObject* object) = 0;
    virtual std::vector<Collider*> GetColliders() = 0;
    virtual glm::vec2* GetWorldPosition() = 0;
};

