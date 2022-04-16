#include "Vegetable.h"

Vegetable:: Vegetable(VegetableType vegetable){
	vtype = vegetable;
}

Vegetable::~Vegetable() {}

//implement virtual function of Interface Interactable
bool canInteract(Player* player) {
	return true; //TODO: return true by default 
}

void interact(Player* player) {

}
