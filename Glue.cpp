#include "Glue.h"
#include "GameManager.h"

Glue::Glue()
	: GardenTool(GLUE)
{
	hold_transformation_ = glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));
}

bool Glue::CanInteract(Player* player) {
	fprintf(stderr, "Checking if glue can be interacted with: %d\n", !player->GetIsHolding());
	return !player->GetIsHolding();
}

void Glue::FixedUpdate()
{
	if (playerBeingTrapped) {
		if (playerBeingTrapped->glueTime > maxGlueTime) {
			playerBeingTrapped->isGlued = false;
			playerBeingTrapped->glueTime = 0;
			GameManager::RemoveEntities({ this });
		}
		else {
			const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
			playerBeingTrapped->glueTime += delta;
		}
	}
}

void Glue::OnInteract(Player* player) {
	fprintf(stderr, "Glue is being interacted with\n");
	if (holding_player != nullptr) {
		holding_player->Drop();
	}
	SetHeight(pickupHeight);
	player->SetHoldEntity(this);
	player->SetTriggeringEntity(nullptr);

	isHeld = true;
	holding_player = player;
}

void Glue::OnDrop() {
	SetHeight(dropHeight);
	isHeld = false;
	holding_player = nullptr;
}

glm::mat4 Glue::GetHoldTransform() {
	return hold_transformation_;
}

//void Glue::OnCollide(PhysicsObject* object) {
//	fprintf(stderr, "Starting glue on collide\n");
//	if(isOnGround){
//		auto player = dynamic_cast<Player*>(object);
//		if (player && playerBeingTrapped == nullptr) {
//			playerBeingTrapped = player;
//			player->isGlued = true;
//			player->glueTime = 0;
//		}
//	}
//	fprintf(stderr, "Ending glue on collide\n");
//}
