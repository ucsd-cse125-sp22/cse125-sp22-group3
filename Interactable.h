#include "Player.h"

class Interactable {
	public:
		virtual bool canInteract(Player* player) {};
		virtual void interact(Player* player) {};

		// Is this holdable
		virtual bool isHoldable;
};