#pragma once

class CollidableAABB;

class Collidable
{
public:
    virtual ~Collidable() = default;
    virtual void OnCollide() = 0;
    virtual bool CollidesWith(Collidable* other_collidable) const = 0;
    virtual CollidableAABB* BoundingBox() = 0;

    virtual enum Shape{CIRCLE, AABB_SHAPE} GetColliderShape() = 0;
    virtual bool GetColliderIsStatic() = 0; // Is Object Static? (Stationary)
};

