#include "ColliderAABB.h"

ColliderAABB::ColliderAABB() = default;

ColliderAABB::ColliderAABB(const glm::vec2& min, const glm::vec2& max, const bool is_static)
{
    minimum = min;
    maximum = max;
    collider_is_static = is_static;
}

bool ColliderAABB::CollidesWith(Collider* other_collider) const
{
    //TODO Perform a sub-case for AABB - AABB collision

    //TODO Perform a sub-case for AABB - Circle collision
    
    return false;
}
ColliderAABB* ColliderAABB::BoundingBox()
{
    return this;
}

Collider::Shape ColliderAABB::GetColliderShape()
{
    return AABB_SHAPE;
}

bool ColliderAABB::GetColliderIsStatic()
{
    return collider_is_static;
}

ColliderAABB* ColliderAABB::SurroundingBox(const ColliderAABB& first, const ColliderAABB& second)
{
    const glm::vec2 small(fminf(first.minimum[0],second.minimum[0]),
                          fminf(first.minimum[1],second.minimum[1]));

    const glm::vec2 big(fmaxf(first.maximum[0],second.maximum[0]),
                        fmaxf(first.maximum[1],second.maximum[1]));

    return new ColliderAABB(small,big,
        first.collider_is_static && second.collider_is_static);
}