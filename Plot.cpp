#include "Plot.h"
#include "GameManager.h"

Plot::Plot(ModelEnum curr) {
	model = curr;

	translate = new glm::vec2(0.f, 0.f);
	rotation = glm::mat4(1);
	
	// Change to AABB later
	collider_ = new ColliderCircle(glm::vec2(0, 0), 5, false);
	collider_->collider_is_trigger = true;
}

Plot::~Plot() {}
void Plot::FixedUpdate() {
	
	collider_->center = *translate;
}

glm::mat4 Plot::GetTransformation() {
	const glm::mat4 trans = glm::translate(glm::vec3((*translate)[0], 0, -(*translate)[1])) * rotation;
	return trans;
}

std::vector<Collider*> Plot::GetColliders() {
	return { collider_ };
}

glm::vec2* Plot::GetWorldPosition() {
	return translate;
}

glm::vec3 Plot::GetPosition() const {
	return glm::vec3((*translate)[0], 0, -(*translate)[1]);
}

glm::mat4 Plot::GetParentTransform()
{
	return GetTransformation() * rotation;
}
ModelEnum Plot::GetModelEnum() { return model; }
AniMode Plot::GetAniMode() { return modelAnim; }

void Plot::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
}

void Plot::SetRotation(glm::mat4 rotation) {
	this->rotation = rotation;
}

bool Plot::CanInteract(Player* player) {
	auto seed = dynamic_cast<Seed*>(player->GetHoldEntity());

	// WARNING: SPAGHETTI (ASK LEON OR DANICA ABOUT IT)
	// can interact if:
	// 1. player is holding a seed and plot is empty
	// 2. player is holding nothing and plot is harvestable
	bool can_plant_seed = (player->GetIsHolding() && seed && !isPlanted);
	bool can_harvest_veggie = (!player->GetIsHolding() && plantedVegetable && plantedVegetable->isHarvestable);
	return  can_plant_seed || can_harvest_veggie;
	//return true;
}

void Plot::OnInteract(Player* player) {
	if (player->isHolding && !isPlanted) {
		if (auto seed = dynamic_cast<Seed*>(player->GetHoldEntity())) {

			SetPlantedVegetable(seed);

			VeggieInfo veggie = veggie_map[seed->GetType()];
			seed->SetModel(veggie.flag_model, GetTranslate());
			seed->SetPlanted();
			player->Drop();
			player->SetTriggeringEntity(nullptr);
		}
		else {
			printf("Warning: You can only plant seeds not veggies bro\n");
		}
	}
	else if (!player->isHolding) {
		Seed* seed = plantedVegetable;
		if (seed != nullptr && plantedVegetable->isHarvestable) {
			// Remove the seed
			VegetableType veggieType = GetPlantedVegetable();
			auto seed_ = dynamic_cast<GameEntity*>(seed);
			if (seed_ != nullptr) {
				GameManager::RemoveEntities({ seed_ });
				SetPlantedVegetable(nullptr);
			}

			Vegetable* veggie = nullptr;
			// Spawn the correct vegetable on the player
			VeggieInfo veggie_info = veggie_map[seed->GetType()];
			veggie = new Vegetable{ seed->GetType(), veggie_info.veggie_model };
			GameManager::AddEntities({ veggie });
			player->SetHoldEntity(veggie);
			player->SetTriggeringEntity(nullptr);
		}
	}
}

void Plot::SetPlantedVegetable(Seed* seed) {
	isPlanted = seed != nullptr;
	plantedVegetable = seed;
}

VegetableType Plot::GetPlantedVegetable() {
	//return plantedVegetable;
	//TODO DONT FORGET
	if (plantedVegetable == nullptr)
		return VegetableType::NOVEGGIE;
	return plantedVegetable->GetType();
}

glm::vec2 Plot::GetTranslate()
{
	return *translate;
}
