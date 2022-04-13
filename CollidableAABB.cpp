#include "CollidableAABB.h"

CollidableAABB::CollidableAABB() = default;

CollidableAABB::CollidableAABB(const glm::vec2& min, const glm::vec2& max, bool is_static)
{
    minimum = min;
    maximum = max;
    collider_is_static = is_static;
}

bool CollidableAABB::CollidesWith(Collidable* other_collidable) const
{
    //TODO Perform a sub-case for AABB - AABB collision

    //TODO Perform a sub-case for AABB - Circle collision
    return false;
}
CollidableAABB* CollidableAABB::BoundingBox()
{
    return this;
}

Collidable::Shape CollidableAABB::GetColliderShape()
{
    return AABB_SHAPE;
}

bool CollidableAABB::GetColliderIsStatic()
{
    //TODO
    return false;
}

CollidableAABB* CollidableAABB::SurroundingBox(const CollidableAABB& first, const CollidableAABB& second)
{
    const glm::vec2 small(fminf(first.minimum[0],second.minimum[0]),
                          fminf(first.minimum[1],second.minimum[1]));

    const glm::vec2 big(fmaxf(first.maximum[0],second.maximum[0]),
                        fmaxf(first.maximum[1],second.maximum[1]));

    return new CollidableAABB(small,big,
        first.collider_is_static && second.collider_is_static);
}