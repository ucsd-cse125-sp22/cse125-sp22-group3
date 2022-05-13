#pragma once
#include "SabotageItem.h"
class Shovel : public SabotageItem
{
private:
	float pickupHeight = -1; // TODO hello
	float dropHeight = -4; // TODO danica
	glm::mat4 hold_transformation_{}; // TODO ici

public:
	Shovel();

	// Interactable
	bool Shovel::CanInteract(Player* player);
	void Shovel::OnInteract(Player* player);

	// Holdable
	void Shovel::OnDrop();
	glm::mat4 Shovel::GetHoldTransform();

};

