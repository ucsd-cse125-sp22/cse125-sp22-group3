#pragma once
#include <glm/vec2.hpp>

#include "Collider.h"

class ColliderCircle : public Collider
{
public:
    ColliderCircle();
    ColliderCircle(glm::vec2 center, float radius, bool is_static = true);

    glm::vec2 center{};
    float radius{};
    bool collider_is_static{};
     
    bool CollidesWith(Collider* other_collider) const override;
    ColliderAABB* BoundingBox() override;
    
    Collider::Shape GetColliderShape() override;
    bool GetColliderIsStatic() override;
};
