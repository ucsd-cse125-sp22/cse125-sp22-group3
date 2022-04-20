﻿#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine(const std::vector<PhysicsObject*>& phys_objects)
{
	// Add physic objects to lists
	for (PhysicsObject* object : phys_objects)
	{
		//TODO fix this
		if(object->GetColliders()[0]->GetColliderIsStatic())
		{
			static_collidables_.push_back(object);
		}
		else
		{
			moving_collidables_.push_back(object);
		}
	}
}

void PhysicsEngine::Compute()
{
	// TODO static collidables in a BVH

	// Compute Moving Collidables with other Moving Collidables
	for (int i = 0; i < moving_collidables_.size(); i++)
	{
		for (int j = i + 1; j < moving_collidables_.size(); j++)
		{
			PhysicsObject* object_1 = moving_collidables_[i];
			PhysicsObject* object_2 = moving_collidables_[j];

			//TODO fix this to be more general for objects with more than one collider
			Collider* collider_1 = object_1->GetColliders()[0];
			Collider* collider_2 = object_2->GetColliders()[0];
			if (collider_1->CollidesWith(collider_2))
			{
				object_1->OnCollide();
				object_2->OnCollide();

				ResolveCollision(object_1, object_2);
			}
		}
	}
}

inline void ResolveCircleToAABBCollision(ColliderCircle* circle, ColliderAABB* aabb, glm::vec2* circle_pos, glm::vec2* aabb_pos) {
	const float x = fmaxf(aabb->minimum[0], fminf(circle->center[0], aabb->maximum[0]));
	const float y = fmaxf(aabb->minimum[1], fminf(circle->center[1], aabb->maximum[1]));

	const glm::vec2 closest_aabb_point{ x,y };
	const glm::vec2 center_aabb = (aabb->maximum + aabb->minimum) / 2.f;
	// Get distance to move from closest point on AABB to sphere.
	const float dist_to_move = circle->radius + glm::length(closest_aabb_point - center_aabb) - glm::length(circle->center - center_aabb);
	const glm::vec2 dir_to_move = glm::normalize(circle->center - center_aabb) * dist_to_move / 2.f;

	*circle_pos += dir_to_move;
	*aabb_pos -= dir_to_move;

}

inline void ResolveCircleToCircleCollision(ColliderCircle* circle_1, ColliderCircle* circle_2, glm::vec2* pos_1, glm::vec2* pos_2) {
	float dist = glm::length(circle_1->center - circle_2->center);
	float dist_to_move = circle_1->radius + circle_2->radius - dist;
	glm::vec2 dir_to_move = glm::normalize(circle_1->center - circle_2->center) * dist_to_move / 2.f;

	*pos_1 += dir_to_move;
	*pos_2 -= dir_to_move;
}

//TODO I'm not sure if we're even going to need this, but if we add a moving AABB object we potentially would.
inline void ResolveAABBToAABBCollision(ColliderAABB* aabb_1, ColliderAABB* aabb_2, glm::vec2* pos_1, glm::vec2* pos_2) {
	const glm::vec2 center_1 = (aabb_1->maximum + aabb_1->minimum) / 2.f;
	const glm::vec2 center_2 = (aabb_2->maximum + aabb_2->minimum) / 2.f;

	const float x_1 = fmaxf(aabb_1->minimum[0], fminf(center_2[0], aabb_1->maximum[0]));
	const float y_1 = fmaxf(aabb_1->minimum[1], fminf(center_2[1], aabb_1->maximum[1]));

	const float x_2 = fmaxf(aabb_2->minimum[0], fminf(center_1[0], aabb_2->maximum[0]));
	const float y_2 = fmaxf(aabb_2->minimum[1], fminf(center_1[1], aabb_2->maximum[1]));

	const glm::vec2 closest_point_1{ x_1,y_1 };
	const glm::vec2 closest_point_2{ x_2,y_2 };

	const float dist_to_move = glm::length(closest_point_2 - center_2) + glm::length(closest_point_1 - center_1) - glm::length(center_1 - center_2);
	const glm::vec2 dir_to_move = glm::normalize(center_1 - center_2) * dist_to_move / 2.f;

	*pos_1 += dir_to_move;
	*pos_2 -= dir_to_move;
}

inline void ResolvePlayerVegetableCollision(Player* player, Vegetable* vegetable)
{
	// TODO: Get closest trigger collider, maybe store these positions in a list?
	// Eventually move this stuff below when the player presses a key to interact
	if (player->isHolding) return;  // If player is already holding something do nothing

	// I think this is safe to do since it'll only go this function if the second collider is a trigger AKA an interactable
	player->SetTriggeringEntity(vegetable);

}

inline void ResolveTriggerCollision(PhysicsObject* first, PhysicsObject* second) {
	auto player = dynamic_cast<Player*>(first);
	if (player != nullptr) {
		auto vegetable = dynamic_cast<Vegetable*>(second);
		if (vegetable != nullptr) {
			ResolvePlayerVegetableCollision(player, vegetable);
		}
		return;
	}
	player = dynamic_cast<Player*>(second);
	if (player != nullptr) {
		auto vegetable = dynamic_cast<Vegetable*>(first);
		if (vegetable != nullptr) {
			ResolvePlayerVegetableCollision(player, vegetable);
		}
		return;
	}
}

//TODO take in Colliders instead of Physics Objects
void PhysicsEngine::ResolveCollision(PhysicsObject* first, PhysicsObject* second)
{
	Collider* col_1 = first->GetColliders()[0];
	Collider* col_2 = second->GetColliders()[0];

	if (col_1->GetColliderIsTrigger() || col_2->GetColliderIsTrigger()) {
		ResolveTriggerCollision(first, second);
		return;
	}
	
	glm::vec2* pos_1 = first->GetWorldPosition();
	glm::vec2* pos_2 = second->GetWorldPosition();

	if (col_1->GetColliderShape() == Collider::CIRCLE)
	{
		const auto circle_1 = dynamic_cast<ColliderCircle*>(col_1);
		if (col_2->GetColliderShape() == Collider::CIRCLE)
		{
			const auto circle_2 = dynamic_cast<ColliderCircle*>(col_2);
			ResolveCircleToCircleCollision(circle_1, circle_2, pos_1, pos_2);
		}
		if (col_2->GetColliderShape() == Collider::AABB_SHAPE) {
			const auto aabb = dynamic_cast<ColliderAABB*>(col_2);
			ResolveCircleToAABBCollision(circle_1, aabb, pos_1, pos_2);
		}
	}
	if (col_1->GetColliderShape() == Collider::AABB_SHAPE) 
	{
		const auto aabb_1 = dynamic_cast<ColliderAABB*>(col_1);

		if (col_2->GetColliderShape() == Collider::CIRCLE) {
			const auto circle = dynamic_cast<ColliderCircle*>(col_2);
			ResolveCircleToAABBCollision(circle, aabb_1, pos_2, pos_1);
		}
		if (col_2->GetColliderShape() == Collider::AABB_SHAPE) {
			const auto aabb_2 = dynamic_cast<ColliderAABB*>(col_2);
			ResolveAABBToAABBCollision(aabb_1, aabb_2, pos_1, pos_2);
		}
	}

}


