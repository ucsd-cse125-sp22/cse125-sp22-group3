#pragma once

class ColliderAABB;

class PhysicsObject
{
public:
    virtual ~PhysicsObject() = default;
    virtual void OnCollide() = 0;
};

