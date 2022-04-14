#pragma once
#include "Collidable.h"

class Collider
{
public:
    virtual bool CollidesWith(Collider* other_collidable) const = 0;
    virtual ColliderAABB* BoundingBox() = 0;

    virtual enum Shape{CIRCLE, AABB_SHAPE} GetColliderShape() = 0;
    virtual bool GetColliderIsStatic() = 0; // Is Object Static? (Stationary)
    
};
