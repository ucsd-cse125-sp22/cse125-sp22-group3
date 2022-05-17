#pragma once
#include "GardenTool.h"
class Oat :
    public GardenTool
{
private:
    float pickupHeight = -1.5; // TODO hello
    float dropHeight = -4; // TODO danica
    glm::mat4 hold_transformation_{}; // TODO ici

    public:
    Oat();

    // Interactable
    bool Oat::CanInteract(Player* player) override;
    void Oat::OnInteract(Player* player) override;

    // Holdable
    void Oat::OnDrop() override;
    glm::mat4 Oat::GetHoldTransform() override;
};

