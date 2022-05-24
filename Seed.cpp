#include "Seed.h"
#include "GameManager.h"

Seed::Seed(VegetableType vegetable, ModelEnum curr) {
	type = vegetable;
	modelAnim = NO_ANI;
	model = curr;

	translate = new glm::vec2(0.f, 0.f);
	translate3D = new glm::vec3(0.f, 0.f, 0.f);
	rotation = glm::mat4(1);

	collider_ = new ColliderCircle(glm::vec2(0, 0), 5, false);
	collider_->collider_is_trigger = true;

	hold_transformation_ = glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));
	requiresWater = veggie_map[type].requires_water;
	requiresFertilizer = veggie_map[type].requires_fertilizer;
}

Seed::~Seed() {
	delete collider_;
	delete translate;
	delete translate3D;
	GameManager::RemoveEntities({ indicate_product });
}

void Seed::waterSeed()
{
	
	if (requiresWater)
	{
		if (requiresFertilizer) {
			auto indicate_ = dynamic_cast<Indicator*>(indicate_product);
			indicate_->SetFertilize();
		}
		else {
			GameManager::RemoveEntities({ indicate_product });
			indicate_product = nullptr;
		}
	}
	requiresWater = false;
}

void Seed::fertilizeSeed()
{
	
	if (requiresFertilizer)
	{
		if (requiresWater) {
			auto indicate_ = dynamic_cast<Indicator*>(indicate_product);
			indicate_->SetWater();
		}
		else {
			GameManager::RemoveEntities({ indicate_product });
			indicate_product = nullptr;
		}
	}
	requiresFertilizer = false;
}

void Seed::FixedUpdate()
{
	bool needsWater = veggie_map[type].requires_water;
	bool needsFert = veggie_map[type].requires_fertilizer;
	if (isPlanted && !isPoisoned && !requiresWater && !requiresFertilizer) { // stop growth if poisoned
		plantedTime += GameManager::GetFixedDeltaTime();
		if (!isHarvestable && getIsReady()) {
			glow_particle = new Particle(PARTICLE_GLOW);
			auto particle_ = dynamic_cast<Particle*>(glow_particle);
			particle_->modelAnim = PARTICLE_PLAY;
			particle_->SetPosition(glm::vec3((*translate)[0], particle_->glowParticleHeight, -(*translate)[1]));
			GameManager::AddEntities({ glow_particle });

			isHarvestable = true;
			VeggieInfo veggie = veggie_map[type];
			model = veggie.veggie_model;
		}
	}
	collider_->center = *translate;
}

glm::mat4 Seed::GetParentTransform()
{
	return GetTranslation() * rotation;
}

ModelEnum Seed::GetModelEnum() { return model; }
AniMode Seed::GetAniMode() { return modelAnim; }

void Seed::SetModel(ModelEnum newModel, glm::vec3 pos)
{
	model = newModel;
	*translate = glm::vec2(pos.x, pos.z);
	*translate3D = pos;
	//*translate3D = pos;
	// rotate 90 degrees towards viewer
	rotation = glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(0,1,0));


}

std::vector<Collider*> Seed::GetColliders()
{
	return { collider_ };
}

glm::vec2* Seed::GetWorldPosition()
{
	return translate;
}

glm::vec3 Seed::GetPosition() const
{
	return glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2]);
}

void Seed::SetPosition(glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
	*translate3D = glm::vec3(position[0], position[1], -position[2]);
}

void Seed::SetRotation(glm::mat4 rotation)
{
	this->rotation = rotation;
}

glm::mat4 Seed::GetTranslation() {
	return glm::translate(glm::vec3((*translate3D)[0],(*translate3D)[1], -(*translate3D)[2]));
}

glm::mat4 Seed::GetRotation() const
{
	return rotation;
}

VegetableType Seed::GetType()
{
	return type;
}

bool Seed::getIsReady()
{
	VeggieInfo veggie = veggie_map[type];
	if (plantedTime >= veggie.growth_time) return true;
	return false;
}

void Seed::SetHeight(float height)
{
	(*translate3D)[1] = height;
}

float Seed::GetHeight()
{
	return (*translate3D)[1];
}

bool Seed::CanInteract(Player* player) { 
	return !player->GetIsHolding() && !isPlanted; 
}

void Seed::OnInteract(Player* player) { 
	if (holding_player != nullptr) {
		holding_player->Drop();
	}
	SetHeight(pickupHeight);
	player->SetHoldEntity(this);
	player->SetTriggeringEntity(nullptr);

	isHeld = true;
	holding_player = player;
}

void Seed::OnDrop() { 
	if(!isPlanted)
		SetHeight(dropHeight);
	isHeld = false;
	holding_player = nullptr;
}


glm::mat4 Seed::GetHoldTransform() { 
	return hold_transformation_; 
}

void Seed::SetPlanted()
{
	isPlanted = true;
	plantedTime = 0;

	if ((veggie_map[type].requires_water || veggie_map[type].requires_fertilizer)) {
		indicate_product = new Indicator(type);
		auto indicator_ = dynamic_cast<Indicator*>(indicate_product);
		indicator_->SetPosition(glm::vec3((*translate)[0], indicatorHeight, -(*translate)[1]));
		GameManager::AddEntities({ indicate_product });
	}
}

glm::mat4 Seed::GetTransformation() {
	const glm::mat4 trans = glm::translate(glm::vec3((*translate3D)[0], (*translate3D)[1], -(*translate3D)[2])) * rotation;
	return isHeld ? trans * hold_transformation_ : trans;
}

