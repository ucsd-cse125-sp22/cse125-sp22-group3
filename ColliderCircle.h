#pragma once
#include "glm.h"

#include "Collider.h"

class ColliderCircle : public Collider
{
public:
    ColliderCircle();
    ColliderCircle(glm::vec2 center, float radius, bool is_static = true);
    ~ColliderCircle();

    glm::vec2 center{};
    float radius{};
    bool collider_is_static{};
    bool collider_is_trigger = false;
     
    bool CollidesWith(Collider* other_collider) const override;
    ColliderAABB* BoundingBox() override;
    
    Collider::Shape GetColliderShape() override;
    bool GetColliderIsStatic() override;
    bool GetColliderIsTrigger() override;
};
