#include "ColliderCircle.h"
#include "ColliderAABB.h"

ColliderCircle::ColliderCircle(const glm::vec2 center, const float radius, const bool is_static)
{
    this->center = center;
    this->radius = radius;
    this->collider_is_static = is_static;
}

bool ColliderCircle::CollidesWith(Collider* other_collider) const
{
    //TODO Perform a sub-case for Circle - Circle collision

    
    //TODO Perform a sub-case for Circle - AABB collision

    
}

ColliderAABB* ColliderCircle::BoundingBox()
{
    return new ColliderAABB(center - radius, center + radius, collider_is_static);
}

Collider::Shape ColliderCircle::GetColliderShape()
{
    
}

bool ColliderCircle::GetColliderIsStatic()
{
    return collider_is_static;
}