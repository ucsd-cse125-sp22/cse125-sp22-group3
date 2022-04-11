#include "Interactable.h"

enum VegetableType {
    TOMATO,
    CABBAGE,
    CORN,
    CARROT,
    RADISH
};

class Vegetable : public Interactable {
    public:
        Vegetable(VegetableType type);
        ~Vegetable();

};