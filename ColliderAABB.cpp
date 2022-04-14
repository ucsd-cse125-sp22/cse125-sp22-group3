#include "ColliderAABB.h"

#include <glm/detail/func_geometric.inl>

#include "ColliderCircle.h"

ColliderAABB::ColliderAABB() = default;

ColliderAABB::ColliderAABB(const glm::vec2& min, const glm::vec2& max, const bool is_static)
{
    minimum = min;
    maximum = max;
    collider_is_static = is_static;
}

bool ColliderAABB::CollidesWith(Collider* other_collider) const
{
    switch (other_collider->GetColliderShape()) {
        case AABB_SHAPE: {
            const auto other_aab = dynamic_cast<ColliderAABB*>(other_collider);
            //Kooky! Crazy!
            return  minimum[0] <= other_aab->minimum[0] && maximum[0] >= other_aab->minimum[0] &&
                    minimum[1] <= other_aab->minimum[1] && maximum[1] >= other_aab->minimum[1];
        }
        case CIRCLE: {
            const auto circle = dynamic_cast<ColliderCircle*>(other_collider);
            const float x = fmaxf(minimum[0], fminf(circle->center[0], maximum[0]));
            const float y = fmaxf(minimum[1], fminf(circle->center[1], maximum[1]));

            // Check if closest point on AABB (x,y) is inside sphere radius.
            return glm::length(glm::vec2(x,y) - circle->center) < circle->radius;
        }
    }
    
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

bool ColliderAABB::GetColliderIsTrigger()
{
    return collider_is_trigger;
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