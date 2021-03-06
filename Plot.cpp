#include "Plot.h"
#include "GameManager.h"

Plot::Plot(ModelEnum curr) {
	model = curr;

	translate = new glm::vec2(0.f, 0.f);
	translate3D = new glm::vec3(0.f, plotHeight, 0.f);
	rotation = glm::mat4(1);
	
	// Change to AABB later
	collider_ = new ColliderCircle(glm::vec2(0, 0), 5, false);
	collider_->collider_is_trigger = true;
}

Plot::~Plot()
{
	delete translate;
	delete translate3D;
	delete collider_;
}

void Plot::FixedUpdate() {
	
	collider_->center = *translate;
}

glm::mat4 Plot::GetTransformation() {
	const glm::mat4 trans = glm::translate(glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2])) * rotation;
	return trans;
}

std::vector<Collider*> Plot::GetColliders() {
	return { collider_ };
}

glm::vec2* Plot::GetWorldPosition() {
	return translate;
}

glm::vec3 Plot::GetPosition() const {
	return glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[1]);
}

glm::mat4 Plot::GetParentTransform()
{
	return GetTransformation() * rotation;
}
ModelEnum Plot::GetModelEnum() { return model; }
AniMode Plot::GetAniMode() { return modelAnim; }

void Plot::SetPosition(glm::vec3 position)
{
	position[1] = plotHeight;
	*translate = glm::vec2(position[0], -position[2]);
	*translate3D = glm::vec3(position[0], position[1], -position[2]);
}

void Plot::SetRotation(glm::mat4 rotation) {
	this->rotation = rotation;
}

bool Plot::CanInteract(Player* player) {
	auto seed = dynamic_cast<Seed*>(player->GetHoldEntity());
	auto tool = dynamic_cast<GardenTool*>(player->GetHoldEntity());

	// WARNING: SPAGHETTI (ASK LEON OR DANICA ABOUT IT)
	// can interact if:
	// 1. player is holding a seed and plot is empty
	// 2. player is holding nothing and plot is harvestable
	bool can_use_tool = (player->GetIsHolding() && tool);
	bool can_plant_seed = (player->GetIsHolding() && seed && !isPlanted);
	bool can_harvest_veggie = (!player->GetIsHolding() && plantedVegetable && plantedVegetable->isHarvestable);
	bool can_remove_poison = (plantedVegetable && plantedVegetable->isPoisoned);
	return  can_use_tool || can_plant_seed || can_harvest_veggie || can_remove_poison;
	//return true;
}

void Plot::OnInteract(Player* player) {
	// anyone can destroy the plot with a shovel
	if (auto shovel = dynamic_cast<Shovel*>(player->GetHoldEntity())) {
		player->Drop();
		player->SetTriggeringEntity(nullptr);
		player->sound_shovel = true;
		Seed* seed = plantedVegetable;
		if (seed != nullptr) {
			// TODO: REMOVE THIS IF WE DO DESTRUCTOR CALLS
			if (seed->glow_particle != nullptr) {
				GameManager::RemoveEntities({ seed->glow_particle });
			}
			if (seed->indicate_product != nullptr) {
				GameManager::RemoveEntities({ seed->indicate_product });
			}
			SetPlantedVegetable(nullptr);
			GameManager::RemoveEntities({ seed });			
		}

		GameManager::RemoveEntities({ this, shovel });
	}
	// anyone can poison 
	else if (auto bouteille_poisson = dynamic_cast<Poison*>(player->GetHoldEntity())) {

		Seed* seed = plantedVegetable;
		if (seed != nullptr && !seed->isPoisoned) {
			player->sound_poison = true;
			player->Drop();
			player->SetTriggeringEntity(nullptr);
			GameManager::RemoveEntities({ bouteille_poisson });

			if (seed->glow_particle != nullptr) {
				GameManager::RemoveEntities({ seed->glow_particle });
			}
			if (seed->indicate_product != nullptr) {
				GameManager::RemoveEntities({ seed->indicate_product });
			}
			seed->isPoisoned = true;
			glm::vec3 trans = GetTranslate();
			seed->SetModel(WORLD_FLAG_POISON, glm::vec3(trans[0],poisonFlagHeight, trans[2]));
		}
	}
	else if (auto net = dynamic_cast<VeggieNet*>(player->GetHoldEntity()))
	{
		Seed* seed = plantedVegetable;
		if (seed != nullptr && seed->isHarvestable)
		{
			player->sound_net = true;
			player->Drop();
			player->SetTriggeringEntity(nullptr);
			GameManager::RemoveEntities({net});

			if (seed->glow_particle != nullptr) {
				GameManager::RemoveEntities({ seed->glow_particle });
			}
			if (seed->indicate_product != nullptr) {
				GameManager::RemoveEntities({ seed->indicate_product });
			}

			auto seed_ = dynamic_cast<GameEntity*>(seed);
			Vegetable* veggie = nullptr;

			// Spawn the correct vegetable on the player
			VeggieInfo veggie_info = veggie_map[seed->GetType()];
			veggie = new Vegetable{ seed->GetType(), veggie_info.veggie_model };
			GameManager::AddEntities({ veggie });

			player->SetHoldEntity(veggie);
			player->SetTriggeringEntity(nullptr);

			SetPlantedVegetable(nullptr);
			GameManager::RemoveEntities({ seed });
		}
	}

	// only plot owner can plant and pick up
	else if (player->GetModelEnum() == plot_ownership[this->GetModelEnum()]) {
		if (plantedVegetable && plantedVegetable->isPoisoned) {
			Seed* poisoned_seed = plantedVegetable;
			if (poisoned_seed->glow_particle != nullptr) {
				GameManager::RemoveEntities({ poisoned_seed->glow_particle });
			}

			SetPlantedVegetable(nullptr);
			GameManager::RemoveEntities({ poisoned_seed });
			
			player->SetTriggeringEntity(nullptr);
		}
		else if (player->isHolding) {
			if (auto seed = dynamic_cast<Seed*>(player->GetHoldEntity())) {
				if (!isPlanted)
				{
					player->sound_plant = true;
					SetPlantedVegetable(seed);

					VeggieInfo veggie = veggie_map[seed->GetType()];
					seed->SetModel(veggie.flag_model, GetTranslate());
					seed->SetPlanted();
					player->Drop();
					player->SetTriggeringEntity(nullptr);
				}
			}
			else if (auto wateringCan = dynamic_cast<WateringCan*>(player->GetHoldEntity()))
			{
				if (isPlanted) {
					printf("Is planted: %d\n", isPlanted);
					player->sound_watering = true;
					plantedVegetable->waterSeed();
				}
			}
			else if (auto fertilizer = dynamic_cast<Fertilizer*>(player->GetHoldEntity()))
			{
				//TODO:Check if needs fertilizer then delete
				//if(plantedVegetable->)
				if (isPlanted && plantedVegetable->requiresFertilizer) {
					player->sound_fertilize = true;
					plantedVegetable->fertilizeSeed();
					player->Drop();
					player->SetTriggeringEntity(nullptr);
					GameManager::RemoveEntities({ fertilizer });
				}
			}
		}
		else if (!player->isHolding) {
			Seed* seed = plantedVegetable;
			// remove poisoned seed
			if (seed != nullptr && seed->isPoisoned) {
				auto seed_ = dynamic_cast<GameEntity*>(seed);

				// remove particle
				if (seed->glow_particle != nullptr) {
					GameManager::RemoveEntities({ seed->glow_particle });
				}

				if (seed_ != nullptr) {
					SetPlantedVegetable(nullptr);
					GameManager::RemoveEntities({ seed_ });
				}
			}
			else if (seed != nullptr && plantedVegetable->isHarvestable) {
				// Remove the seed
				VegetableType veggieType = GetPlantedVegetable();
				auto seed_ = dynamic_cast<GameEntity*>(seed);

				// remove particle
				if (seed->glow_particle != nullptr) {
					GameManager::RemoveEntities({ seed->glow_particle });
				}

				Vegetable* veggie = nullptr;
				// Spawn the correct vegetable on the player
				VeggieInfo veggie_info = veggie_map[seed->GetType()];
				veggie = new Vegetable{ seed->GetType(), veggie_info.veggie_model };
				GameManager::AddEntities({ veggie });
				player->sound_harvest = true;

				player->SetHoldEntity(veggie);
				player->SetTriggeringEntity(nullptr);
				veggie->holding_player = player;

				if (seed_ != nullptr) {
					SetPlantedVegetable(nullptr);
					GameManager::RemoveEntities({ seed_ });
				}
			}
		}
	}
	else {
		player->sound_no = true;
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

glm::vec3 Plot::GetTranslate()
{
	return *translate3D;
}
