#include "VeggieNet.h"

VeggieNet::VeggieNet()
	: GardenTool(NET)
{
	hold_transformation_ = glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));
}

bool VeggieNet::CanInteract(Player* player) {
	return !player->GetIsHolding();
}

void VeggieNet::OnInteract(Player* player) {
	if (holding_player != nullptr) {
		holding_player->Drop();
	}
	SetHeight(pickupHeight);
	player->SetHoldEntity(this);
	player->SetTriggeringEntity(nullptr);

	isHeld = true;
	holding_player = player;
}

void VeggieNet::OnDrop() {
	SetHeight(dropHeight);
	isHeld = false;
	holding_player = nullptr;
}

glm::mat4 VeggieNet::GetHoldTransform() {
	return hold_transformation_;
}