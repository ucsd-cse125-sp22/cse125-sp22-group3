#include "Soju.h"

Soju::Soju()
    : GardenTool(SOJU)
{
    hold_transformation_ = glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));
}

bool Soju::CanInteract(Player* player) {
    return !player->GetIsHolding();
}

void Soju::OnInteract(Player* player) {
    if (holding_player != nullptr) {
        holding_player->Drop();
    }
    SetHeight(pickupHeight);
    player->SetHoldEntity(this);
    player->SetTriggeringEntity(nullptr);

    isHeld = true;
    holding_player = player;
}

void Soju::OnDrop() {
    SetHeight(dropHeight);
    isHeld = false;
    holding_player = nullptr;
}

glm::mat4 Soju::GetHoldTransform() {
    return hold_transformation_;
}