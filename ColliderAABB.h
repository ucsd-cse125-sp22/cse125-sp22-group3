#pragma once
#include <glm/vec2.hpp>
#include "Collider.h"

class ColliderAABB final : public Collider
{
public:
    ColliderAABB();
    ColliderAABB(const glm::vec2& min, const glm::vec2& max, bool is_static = true);

    glm::vec2 minimum{};
    glm::vec2 maximum{};
    bool collider_is_static{};
     
    bool CollidesWith(Collider* other_collider) const override;
    ColliderAABB* BoundingBox() override;
    
    Collider::Shape GetColliderShape() override;
    bool GetColliderIsStatic() override;
    
    static ColliderAABB* SurroundingBox(const ColliderAABB& first, const ColliderAABB& second);
};
