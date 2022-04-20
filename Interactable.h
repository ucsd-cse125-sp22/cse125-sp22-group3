#pragma once

#include "Player.h"

class Interactable {
	public:
		virtual bool CanInteract(Player* player) = 0;
		virtual void OnInteract(Player* player) = 0;

		// Is this holdable
		bool isHoldable;
};