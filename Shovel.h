#pragma once
#include "GardenTool.h"
class Shovel : public GardenTool
{
private:
	float pickupHeight = -2; // TODO hello
	float dropHeight = -4; // TODO danica
	glm::mat4 hold_transformation_{}; // TODO ici

public:
	Shovel();

	// Interactable
	bool Shovel::CanInteract(Player* player) override;
	void Shovel::OnInteract(Player* player) override;

	// Holdable
	void Shovel::OnDrop() override;
	glm::mat4 Shovel::GetHoldTransform() override;
};

