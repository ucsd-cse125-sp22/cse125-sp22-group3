#pragma once
#include "GardenTool.h"
class Poison :
    public GardenTool
{
private:
	float pickupHeight = -2; // TODO hello
	float dropHeight = -4; // TODO danica
	glm::mat4 hold_transformation_{}; // TODO ici

public:
	Poison();

	// Interactable
	bool Poison::CanInteract(Player* player) override;
	void Poison::OnInteract(Player* player) override;

	// Holdable
	void Poison::OnDrop() override;
	glm::mat4 Poison::GetHoldTransform() override;
};

