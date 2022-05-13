#pragma once
#include "GardenTool.h"
class VeggieNet :
    public GardenTool
{
private:
	float pickupHeight = -2; // TODO hello
	float dropHeight = -4; // TODO danica
	glm::mat4 hold_transformation_{}; // TODO ici

public:
	VeggieNet();

	// Interactable
	bool VeggieNet::CanInteract(Player* player) override;
	void VeggieNet::OnInteract(Player* player) override;

	// Holdable
	void VeggieNet::OnDrop() override;
	glm::mat4 VeggieNet::GetHoldTransform() override;
};

