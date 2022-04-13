#pragma once
#include <glm/vec2.hpp>
#include "Collidable.h"

class CollidableAABB final : public Collidable
{
public:
    CollidableAABB();
    CollidableAABB(const glm::vec2& min, const glm::vec2& max, bool is_static = true);

    glm::vec2 minimum{};
    glm::vec2 maximum{};
    bool collider_is_static{};
     
    bool CollidesWith(Collidable* other_collidable) const override;
    CollidableAABB* BoundingBox() override;

    //TODO Seperate "Collider Shapes" from "Collidable Game Entities"
    //This object is a Collider, so we don't need this function
    void OnCollide() override {} 

    Collidable::Shape GetColliderShape() override;
    bool GetColliderIsStatic() override;
    
    static CollidableAABB* SurroundingBox(const CollidableAABB& first, const CollidableAABB& second);
};
