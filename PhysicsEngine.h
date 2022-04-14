#pragma once
#include "PhysicsObject.h"
#include <vector>

class BVHNode
{
    
};

class PhysicsEngine
{
public:
    explicit PhysicsEngine(std::vector<PhysicsObject*> collidables);

    void AddCollidable(PhysicsObject* collidable);
    void Compute();

private:
    std::vector<PhysicsObject*> static_collidables_;
    std::vector<PhysicsObject*> moving_collidables_;

    BVHNode root_node;
};
