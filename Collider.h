#pragma once

class ColliderAABB;

class Collider
{
public:
    virtual bool CollidesWith(Collider* other_collidable) const = 0;
    virtual ColliderAABB* BoundingBox() = 0;
    
    enum Shape
    {
        CIRCLE,
        AABB_SHAPE
    };
    
    virtual Shape GetColliderShape() = 0;
    virtual bool GetColliderIsStatic() = 0; // Is Object Static? (Stationary)
    virtual bool GetColliderIsTrigger() = 0; // Is Object A Trigger? (Non-Collidable)

};
