#include "Shovel.h"

Shovel::Shovel()
	: GardenTool(SHOVEL)
{
	hold_transformation_ = glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));
}

bool Shovel::CanInteract(Player* player) {
	return !player->GetIsHolding();
}

void Shovel::OnInteract(Player* player) {
	if (holding_player != nullptr) {
		holding_player->Drop();
		player->sound_steal = true;
	}
	SetHeight(pickupHeight);
	player->SetHoldEntity(this);
	player->SetTriggeringEntity(nullptr);

	isHeld = true;
	holding_player = player;
}

void Shovel::OnDrop() {
	SetHeight(dropHeight);
	isHeld = false;
	holding_player = nullptr;
}

glm::mat4 Shovel::GetHoldTransform() {
	return hold_transformation_;
}