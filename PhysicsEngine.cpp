#include "PhysicsEngine.h"

#include <glm/detail/func_geometric.inl>

#include "ColliderCircle.h"

PhysicsEngine::PhysicsEngine(const std::vector<PhysicsObject*>& phys_objects)
{
	// Add physic objects to lists
	for (PhysicsObject* object : phys_objects)
	{
		if(object->GetCollider()->GetColliderIsStatic())
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

			Collider* collider_1 = object_1->GetCollider();
			Collider* collider_2 = object_2->GetCollider();
			if (collider_1->CollidesWith(collider_2))
			{
				object_1->OnCollide();
				object_2->OnCollide();

				ResolveCollision(object_1, object_2);
			}
		}
	}
}

void PhysicsEngine::ResolveCollision(PhysicsObject* first, PhysicsObject* second)
{
	Collider* col_1 = first->GetCollider();
	Collider* col_2 = second->GetCollider();

	glm::vec2* pos_1 = first->GetWorldPosition();
	glm::vec2* pos_2 = second->GetWorldPosition();

	//TODO AABB Collision Resolution
	if (col_1->GetColliderShape() == Collider::CIRCLE)
	{
		const auto circle_1 = dynamic_cast<ColliderCircle*>(col_1);
		if (col_2->GetColliderShape() == Collider::CIRCLE)
		{
			const auto circle_2 = dynamic_cast<ColliderCircle*>(col_2);

			float dist = glm::length(circle_1->center - circle_2->center);
			float dist_to_move = circle_1->radius + circle_2->radius - dist;
			glm::vec2 dir_to_move = glm::normalize(circle_1->center - circle_2->center) * dist_to_move/2.f;

			*pos_1 += dir_to_move;
			*pos_2 -= dir_to_move;
		}
	}

}


