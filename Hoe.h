#pragma once
#include "GardenTool.h"
class Hoe :
    public GardenTool
{
private:
	float pickupHeight = -2; // TODO hello
	float dropHeight = -4; // TODO danica
	glm::mat4 hold_transformation_{}; // TODO ici

public:
	Hoe();

	// Interactable
	bool Hoe::CanInteract(Player* player) override;
	void Hoe::OnInteract(Player* player) override;

	// Holdable
	void Hoe::OnDrop() override;
	glm::mat4 Hoe::GetHoldTransform() override;
};

