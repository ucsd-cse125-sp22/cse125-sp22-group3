#include "Player.h"

class Interactable {
	public:
		virtual bool canInteract(Player* player) = 0;
		virtual void interact(Player* player) = 0;

		// Is this holdable
		bool isHoldable;
};