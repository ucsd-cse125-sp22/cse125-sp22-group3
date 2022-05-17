#pragma once
#include "GardenTool.h"
class Soju :
    public GardenTool
{
private:
    float pickupHeight = -1.5; // TODO hello
    float dropHeight = -4; // TODO danica
    glm::mat4 hold_transformation_{}; // TODO ici

    public:
    Soju();

    // Interactable
    bool Soju::CanInteract(Player* player) override;
    void Soju::OnInteract(Player* player) override;

    // Holdable
    void Soju::OnDrop() override;
    glm::mat4 Soju::GetHoldTransform() override;
};

