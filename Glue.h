#pragma once
#include "GardenTool.h"
class Glue :
    public GardenTool
{
private:
	float pickupHeight = -2; // TODO hello
	float dropHeight = -4; // TODO danica
	glm::mat4 hold_transformation_{}; // TODO ici

public:
	 Glue();

	// Interactable
	bool  Glue::CanInteract(Player* player) override;
	void  Glue::OnInteract(Player* player) override;

	// Holdable
	void  Glue::OnDrop() override;
	glm::mat4  Glue::GetHoldTransform() override;
	
};

