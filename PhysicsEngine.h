#pragma once
#include "Collidable.h"
#include <vector>

class BVHNode
{
    
};

class PhysicsEngine
{
public:
    explicit PhysicsEngine(std::vector<Collidable*> collidables);

    void AddCollidable(Collidable* collidable);
    void Compute();

private:
    std::vector<Collidable*> static_collidables_;
    std::vector<Collidable*> moving_collidables_;

    BVHNode root_node;
};
