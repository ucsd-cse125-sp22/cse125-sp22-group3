#pragma once
#include "PhysicsObject.h"
#include <vector>
//#include <glm/detail/func_geometric.inl>
#include "ColliderCircle.h"
#include "ColliderAABB.h"
#include "Vegetable.h"

class BVHNode
{
    
};

class PhysicsEngine
{
public:
    PhysicsEngine() = default;
    explicit PhysicsEngine(const std::vector<PhysicsObject*>& phys_objects);

    void AddPhysObject(PhysicsObject* phys_object);
    void ConstructBVH();
    void Compute();
    static void ResolveTriggerCollision(PhysicsObject* first, PhysicsObject* second);
    void ResolveCollision(PhysicsObject* first, PhysicsObject* second);

private:
    std::vector<PhysicsObject*> static_collidables_;
    std::vector<PhysicsObject*> moving_collidables_;

    BVHNode root_node;
};
