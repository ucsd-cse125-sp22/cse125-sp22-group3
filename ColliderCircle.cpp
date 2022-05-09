#include "ColliderCircle.h"

#include "glm.h"

#include "ColliderAABB.h"

ColliderCircle::ColliderCircle(const glm::vec2 center, const float radius, const bool is_static)
{
    this->center = center;
    this->radius = radius;
    this->collider_is_static = is_static;
}
ColliderCircle::~ColliderCircle() = default;

bool ColliderCircle::CollidesWith(Collider* other_collider) const
{
    switch (other_collider->GetColliderShape()) {
        case CIRCLE: {
            const auto other_circle = dynamic_cast<ColliderCircle*>(other_collider);
            return glm::length(this->center - other_circle->center) < this->radius + other_circle->radius;
        }
        case AABB_SHAPE: {
            const auto aabb = dynamic_cast<ColliderAABB*>(other_collider);
            const float x = fmaxf(aabb->minimum[0], fminf(center[0], aabb->maximum[0]));
            const float y = fmaxf(aabb->minimum[1], fminf(center[1], aabb->maximum[1]));

            // Check if closest point on AABB (x,y) is inside sphere radius.
            return glm::length(glm::vec2(x,y) - center) < radius;
        }
    }

    // You would never actually reach this but it gives me a warning if I don't put it smh
    return false;
}

ColliderAABB* ColliderCircle::BoundingBox()
{
    return new ColliderAABB(center - radius, center + radius, collider_is_static);
}

Collider::Shape ColliderCircle::GetColliderShape()
{
    return Shape::CIRCLE;
}

bool ColliderCircle::GetColliderIsStatic()
{
    return collider_is_static;
}

bool ColliderCircle::GetColliderIsTrigger()
{
    return collider_is_trigger;
}
