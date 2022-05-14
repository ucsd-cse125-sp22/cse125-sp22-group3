#pragma once
#include "GardenTool.h"
class WateringCan :
    public GardenTool
{
private:
	float pickupHeight = -2; // TODO hello
	float dropHeight = -4; // TODO danica
	glm::mat4 hold_transformation_{}; // TODO ici

public:
	WateringCan();

	// Interactable
	bool WateringCan::CanInteract(Player* player) override;
	void WateringCan::OnInteract(Player* player) override;

	// Holdable
	void WateringCan::OnDrop() override;
	glm::mat4 WateringCan::GetHoldTransform() override;
};

