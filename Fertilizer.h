#pragma once
#include "GardenTool.h"
class Fertilizer :
    public GardenTool
{
private:
	float pickupHeight = -2; // TODO hello
	float dropHeight = -4; // TODO danica
	glm::mat4 hold_transformation_{}; // TODO ici

public:
	Fertilizer();

	// Interactable
	bool Fertilizer::CanInteract(Player* player) override;
	void Fertilizer::OnInteract(Player* player) override;

	// Holdable
	void Fertilizer::OnDrop() override;
	glm::mat4 Fertilizer::GetHoldTransform() override;
};

