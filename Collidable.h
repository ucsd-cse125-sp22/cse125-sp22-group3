#pragma once

class ColliderAABB;

class Collidable
{
public:
    virtual ~Collidable() = default;
    virtual void OnCollide() = 0;
};

