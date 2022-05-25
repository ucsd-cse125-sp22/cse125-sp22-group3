#include "WateringCan.h"

WateringCan::WateringCan()
	: GardenTool(WATERING_CAN)
{
	hold_transformation_ = glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));
}

bool WateringCan::CanInteract(Player* player) {
	return !player->GetIsHolding();
}

void WateringCan::OnInteract(Player* player) {
	if (holding_player != nullptr) {
		holding_player->Drop();
		player->sound_steal = true;
		holding_player->sound_steal = true;
	}
	SetHeight(pickupHeight);
	player->SetHoldEntity(this);
	player->SetTriggeringEntity(nullptr);

	isHeld = true;
	holding_player = player;
}

void WateringCan::OnDrop() {
	SetHeight(dropHeight);
	isHeld = false;
	holding_player = nullptr;
}

glm::mat4 WateringCan::GetHoldTransform() {
	return hold_transformation_;
}