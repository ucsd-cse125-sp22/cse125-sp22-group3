#include "Interactable.h"
#include "Harvestable.h"

enum VegetableType {
    TOMATO,
    CABBAGE,
    CORN,
    CARROT,
    RADISH
};

class Vegetable : public Interactable, public Harvestable{
private:
    VegetableType vtype;
public:
    Vegetable(VegetableType type);
    ~Vegetable();
};