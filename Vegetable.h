#include "Interactable.h"

class Vegetable : public Interactable {
    public:
        Vegetable(VegetableType type);
        ~Vegetable();

};

enum VegetableType {
    TOMATO,
    CABBAGE,
    CORN,
    CARROT,
    RADISH
};