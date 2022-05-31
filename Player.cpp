#include "Player.h"
#include "Hoe.h"
#include "./UI/GUI.h"
#include "Network/NetworkPacket.h"
#include "GameManager.h"
#include "VeggieNet.h"

Player::Player() {
	translate = new glm::vec2(0.f,0.f);
	rotate = glm::vec3(0.0f);
	scale = glm::vec3(0.0f);

	ColliderCircle *collider = new ColliderCircle(glm::vec2(0,0), 3, false);
	ColliderCircle *collider_trigger = new ColliderCircle(glm::vec2(0,0), 3, false);
	collider_trigger->collider_is_trigger = true;
	colliders_ = {collider, collider_trigger};

	dust_particle = new Particle(PARTICLE_DUST);
	dust_particle->modelAnim = PARTICLE_STOP;
	dust_particle->SetPosition(glm::vec3(( * translate)[0], dust_particle->dustParticleHeight, -(*translate)[1]));
	//GameManager::AddEntities({ dust_particle });
}


Player::Player(ModelEnum curr) : Player() {
	// Set initial values
	model = curr;
	//std::cout << model.getBoneCount() << std::endl;
}

Player::~Player()
{
	delete translate;
	for (ColliderCircle *col : colliders_)
	{
		delete col;
	}
	delete dust_particle;
}

GameEntity* Player::GetHoldEntity()
{
	return entityHeld;
}

void Player::SetRotation(glm::vec3 newRot)
{

	rotate = newRot;
}

void Player::FixedUpdate() {
	if (glm::length(move_input) > 1) move_input = glm::normalize(move_input); 
	const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
	// If no movement is given apply friction (epsilon to account for FP errors)
	if (glm::length(move_input) <= 0 || isGlued) {
		// increment stamina
		float appliedStaminaIncreaseRate = oatTimeRemaining > 0 ? oatStaminaIncreaseRate : baseStaminaIncreaseRate;
		curr_stamina = fmin(curr_stamina + (delta*appliedStaminaIncreaseRate), 100);

		float applied_friction = intoxicationTimeRemaining > 0 ? drunk_friction_ : base_friction_;
		if (glm::length(curr_vel_) < applied_friction * delta)
		{
			curr_vel_ = glm::vec2(0,0);
		}
		else {
			curr_vel_ -= glm::normalize(curr_vel_) * applied_friction * delta;
		}

		if (isHolding) {
			this->modelAnim = IDLE_HOLD;
		}
		else if (!isDancing)
		//else
			this->modelAnim = IDLE;
		else
			this->modelAnim = DANCE;
	}
	else {
		// decrement stamina
		float appliedStaminaIncreaseRate = oatTimeRemaining > 0 ? oatStaminaIncreaseRate : baseStaminaIncreaseRate;
		float appliedStaminaDecreaseRate = oatTimeRemaining > 0 ? oatStaminaDecreaseRate : baseStaminaDecreaseRate;
		if(sprint) {
			curr_stamina = fmax(curr_stamina - (delta * appliedStaminaDecreaseRate), 0);
			dust_particle->modelAnim = PARTICLE_PLAY;
		}
		else {
			curr_stamina = fmin(curr_stamina + (delta * appliedStaminaIncreaseRate), 100);
		}

		// check if enough stamina to run
		if (curr_stamina == 0) {
			dust_particle->modelAnim = PARTICLE_STOP;
			sprint = false;
		}

		// Accelerate in our inputted direction
		// Transform input from 2D to 3D Plane
		float applied_accel = intoxicationTimeRemaining > 0 ? drunk_accel_ : base_accel_;
		curr_vel_ += applied_accel * move_input * delta;
		// Cap our speed at some max velocity
		Vegetable* eggplant = dynamic_cast<Vegetable*>(entityHeld);
		bool holding_eggplant = eggplant != nullptr && eggplant->type == VegetableType::GOLDEN_EGGPLANT;
		float max_vel = holding_eggplant ? max_eggplant_velocity_ : max_velocity_;
		if (sprint) max_vel *= sprint_modifier;
		if (glm::length(curr_vel_) > max_vel * glm::length(move_input)) {
			curr_vel_ = glm::normalize(curr_vel_) * max_vel * glm::length(move_input);
		}
		
		if (isHolding)
			this->modelAnim = IDLE_WALK;
		else
			this->modelAnim = WALK;
		isDancing = false;
	}
	
	if (glm::length(curr_vel_) > 0) {
		Move();
	}
	MoveHeld();

	for (ColliderCircle *collider_ : colliders_)
	{
		collider_->center = *translate;
	}
	
	// move dust particle
	const glm::vec4 particleLocation = glm::vec4(0, dust_particle->dustParticleHeight, -dust_particle->dustParticleZOffset, 1) * GetRotation();
	const glm::mat4 particle_rotation = glm::rotate(rotate.y, glm::vec3(0, 1, 0));
	dust_particle->SetPosition(glm::vec3(translate->x + (particleLocation.x), dust_particle->dustParticleHeight, (-1) * translate->y - (particleLocation.z)));
	dust_particle->SetRotation(particle_rotation);

	if (intoxicationTimeRemaining > 0) {
		intoxicationTimeRemaining -= delta;
	}

	if (oatTimeRemaining > 0) {
		oatTimeRemaining -= delta;
	}
}


void Player::Move() {
	if (GetMoveable()) {
		
		//move player
		const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
		const glm::vec2 distance = delta * curr_vel_;
		*translate += distance;

		rotate.y = atan2(curr_vel_.x, -curr_vel_.y); //TODO fix this potentially
	}
}

glm::mat4 Player::GetParentTransform()
{
	return GetTranslation() * GetRotation() * GetScale();
}
ModelEnum Player::GetModelEnum() { return model; }
AniMode Player::GetAniMode() { return modelAnim; }

std::vector<SoundInfo> Player::GetSounds()
{
	std::vector<SoundInfo> output{};
	if (sound_buy) output.push_back(SoundInfo{SFX_BUY, GetPosition()});
	if (sound_sell) output.push_back(SoundInfo{SFX_SELL, GetPosition()});
	if (sound_plot_placement) output.push_back(SoundInfo{SFX_PLOT_PLACE, GetPosition()});
	if (sound_poison) output.push_back(SoundInfo{ SFX_POISON, GetPosition() });
	if (sound_plant) output.push_back(SoundInfo{ SFX_PLANT, GetPosition() });
	if (sound_harvest) output.push_back(SoundInfo{ SFX_HARVEST, GetPosition() });
	if (sound_eggplant_pickup) output.push_back(SoundInfo{ SFX_EGGPLANT_PICKUP, GetPosition() });
	if (sound_steal) output.push_back(SoundInfo{ SFX_STEAL, GetPosition() });
	if (sound_shovel) output.push_back(SoundInfo{ SFX_SHOVEL, GetPosition() });
	if (sound_watering) output.push_back(SoundInfo{ SFX_WATERING, GetPosition() });
	if (sound_no) output.push_back(SoundInfo{ SFX_NO, GetPosition() });
	if (sound_fertilize) output.push_back(SoundInfo{ SFX_FERTILIZE, GetPosition() });
	if (sound_net) output.push_back(SoundInfo{ SFX_STEAL, GetPosition() });

	return output;
}

bool Player::CanInteract(Player* other_player) {
	bool holds_soju = (other_player->GetIsHolding() && dynamic_cast<Soju*>(other_player->GetHoldEntity()));
	return holds_soju;
}

void Player::OnInteract(Player* other_player) {
	if (auto soju = dynamic_cast<Soju*>(other_player->GetHoldEntity())) 
	{
		other_player->Drop();
		other_player->SetTriggeringEntity(nullptr);
		GameManager::RemoveEntities({ soju });
		
		intoxicationTimeRemaining = maxIntoxicationTime;
	}
}

void Player::OnTrigger(PhysicsObject* object)
{
	auto entity = dynamic_cast<GameEntity*>(object);
	auto interactable = dynamic_cast<Interactable*>(object);

	if (entity != nullptr && entity != entityHeld && interactable && interactable->CanInteract(this)) {
		if (entityTriggered == nullptr) {
			SetTriggeringEntity(entity);
		}
		else {
			const auto phys_entity = dynamic_cast<PhysicsObject*>(entityTriggered);
			const float dist = glm::length(*object->GetWorldPosition() - *GetWorldPosition());
			const float curr_dist = glm::length(*phys_entity->GetWorldPosition() - *GetWorldPosition());
			if (dist < curr_dist) {
				SetTriggeringEntity(entity);
			}
		}
	}
}

std::vector<Collider*> Player::GetColliders()
{
	std::vector<Collider*> to_return;
	for (ColliderCircle *col : colliders_)
	{
		to_return.push_back(col);
	}
	return to_return;
}

glm::vec2* Player::GetWorldPosition()
{
	return translate;
}

void Player::Use() {
	if (auto hoe = dynamic_cast<Hoe*>(entityHeld))
	{
		auto plot = new Plot(plot_ownership[this->GetModelEnum()]);
		const float plotOffset = 7.5f;
		glm::vec4 direction4 = glm::vec4(0, 0, -plotOffset, 1) * GetRotation();
		glm::vec3 direction = glm::vec3(direction4/direction4.w);
		glm::vec3 plotPosition = glm::vec3((*translate)[0], -4, -(*translate)[1]) + glm::vec3(direction.x, direction.y, -direction.z);
		plot->SetPosition(plotPosition);
		GameManager::AddEntities({plot});

		Drop();
		GameManager::RemoveEntities({hoe});

		sound_plot_placement = true;
	}
	else if (auto glue = dynamic_cast<Glue*>(entityHeld)) {
		auto glueOnGround = new GlueOnGround();

		const float glueOffset = 9.5f;
		glm::vec4 direction4 = glm::vec4(0, 0, -glueOffset, 1) * GetRotation();
		glm::vec3 direction = glm::vec3(direction4 / direction4.w);
		glm::vec3 gluePosition = glm::vec3((*translate)[0], -4, -(*translate)[1]) + glm::vec3(direction.x, direction.y, -direction.z);;
		glueOnGround->SetPosition(gluePosition);
		GameManager::AddEntities({ glueOnGround });
		glueOnGround->ownerOfGlue = this; // set owner
		Drop();
		GameManager::RemoveEntities({ glue });
	}
	else if (auto oatmeal = dynamic_cast<Oat*>(entityHeld)) {
		Drop();
		GameManager::RemoveEntities({ oatmeal });

		oatTimeRemaining = maxOatTime;
	}
	else {
		auto entity = dynamic_cast<PhysicsObject*>(entityTriggered);
		if (entity != nullptr) {
			Collider* this_trigger_col;
			Collider* that_trigger_col;

			for (Collider *c : this->GetColliders())
			{
				if (c->GetColliderIsTrigger())
				{
					this_trigger_col = c;
					break;
				}
			}

			for (Collider *c : entity->GetColliders())
			{
				if (c->GetColliderIsTrigger())
				{
					that_trigger_col = c;
					break;
				}
			}
			
			if (this_trigger_col && that_trigger_col && this_trigger_col->CollidesWith(that_trigger_col))
			{
				auto interactable = dynamic_cast<Interactable*>(entityTriggered);
				if (interactable != nullptr && interactable->CanInteract(this))
				{
					interactable->OnInteract(this);
				}
			}
		}
	}
}

void Player::Drop() {
	auto holdable = dynamic_cast<Holdable*>(entityHeld);
	if (holdable != nullptr) {
		holdable->OnDrop();
	}
	isHolding = false;
	entityHeld = nullptr;
}

void Player::Dance() {
	if (!isHolding)
		isDancing = true;
}

void Player::Buy(VegetableType bought_vegetable) {
	
	VeggieInfo veggie = veggie_map[bought_vegetable];
	if (!isHolding && veggie.seed_price <= curr_balance) {
		curr_balance -= veggie.seed_price;
		isHolding = true;

		// Spawn the correct vegetable on the player
		Seed* bought_seed = new Seed{ bought_vegetable, veggie.seed_model };
		GameManager::AddEntities({ bought_seed });
		SetHoldEntity(bought_seed);
		bought_seed->holding_player = this;
		bought_seed->isHeld = true;
		sound_buy = true;
	}
	CloseUI();
	//printf("BUYING VEGGIE %f\n", curr_balance);
}

void Player::Buy(ModelEnum tool) {

	ToolInfo tool_ = tool_map[tool];
	if (!isHolding && tool_.price <= curr_balance) {
		curr_balance -= tool_.price;
		isHolding = true;

		// Spawn the correct tool
		GardenTool* tempEntity = nullptr;
		switch (tool) {
			case ModelEnum::NET:
				tempEntity = new VeggieNet();
				break;
			case ModelEnum::SHOVEL:
				tempEntity = new Shovel();
				break;
			case ModelEnum::POISON:
				tempEntity = new Poison();
				break;
			case ModelEnum::WATERING_CAN:
				tempEntity = new WateringCan();
				break;
			case ModelEnum::FERTILIZER:
				tempEntity = new Fertilizer();
				break;
			case ModelEnum::GLUE:
				tempEntity = new Glue();
				break;
			case ModelEnum::SOJU:
				tempEntity = new Soju();
				break;
			case ModelEnum::OATS:
				tempEntity = new Oat();
				break;
			case ModelEnum::HOE:
				tempEntity = new Hoe();
				break;

			default: break;

		}
		
		GameManager::AddEntities({ tempEntity });
		SetHoldEntity(tempEntity);
		tempEntity->holding_player = this;
		tempEntity->isHeld = true;
		sound_buy = true;
	}
	CloseUI();
	//printf("BUYING VEGGIE %f\n", curr_balance);
}

void Player::Sell(){
	if (!isHolding)
		return;
	// TODO: Check if NPC is interactable... unless we don't have to do that?
	if (auto vegetable = dynamic_cast<Vegetable*>(entityHeld)) {
		//SetTriggeringEntity(nullptr);
		if (vegetable == nullptr)
			return;
		VeggieInfo veggie = veggie_map[vegetable->type];
		curr_balance += veggie.sell_price;

		//Drop but with remove :)))
		auto holdable = dynamic_cast<Holdable*>(entityHeld);
		if (holdable != nullptr) {
			holdable->OnDrop();
		}
		GameManager::RemoveEntities({ entityHeld });
		entityHeld = nullptr;
		isHolding = false;

		sound_sell = true;
	}
	//printf("SELLING VEGGIE %f\n", curr_balance);
	CloseSaleUI();
}

void Player::SetSprint(bool sprinting) {
	
	// ending sprint
	if (sprint && !sprinting) {
		dust_particle->modelAnim = PARTICLE_STOP;
	}
	
	sprint = sprinting;

}

void Player::CloseUI()
{
	ui_open = false;
	moveable = true;
	GUI::sale_tools = false;
}

void Player::OpenUI()
{
	ui_open = true;
	moveable = false;
}

void Player::OpenSaleUI()
{
	if (auto vegetable = dynamic_cast<Vegetable*>(entityHeld)) {
		//SetTriggeringEntity(nullptr);
		if (vegetable != nullptr) {
			sale_confirm_ui_open = true;
			moveable = false;
		}
	}

}

void Player::CloseSaleUI() {
	sale_confirm_ui_open = false;
	moveable = true;
}

void Player::EnableMovement()
{
	moveable = true;
}

void Player::DisableMovement()
{
	moveable = false;
}

bool Player::GetMoveable()
{
	return moveable;
}

void Player::ResetSoundBools()
{
	sound_buy = false;
	sound_sell = false;
	sound_plot_placement = false;
	sound_poison = false;
	sound_plant = false;
	sound_harvest = false;
	sound_eggplant_pickup = false;
	sound_steal = false;
	sound_shovel = false;
	sound_watering = false;
	sound_no = false;
	sound_fertilize = false;
	sound_net = false;
}

glm::mat4 Player::GetRotation() {
	/*
	return glm::rotate(rotate.z, glm::vec3(0.0f, 0.0f, 1.0f)) 
		* glm::rotate(rotate.y, glm::vec3(0.0f, 1.0f, 0.0f)) 
		* glm::rotate(rotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
	*/
	return glm::rotate(rotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Player::GetTranslation() {
	return glm::translate(glm::vec3((* translate)[0], playerHeight, -(*translate)[1]));
}

glm::mat4 Player::GetScale() {
	return glm::translate(scale);
}

glm::vec3 Player::GetPosition() const
{
	return glm::vec3((*translate)[0], 0, -(*translate)[1]);
}

void Player::SetWorldPosition(const glm::vec3 position)
{
	*translate = glm::vec2(position[0], -position[2]);
}

void Player::SetTriggeringEntity(GameEntity* entity)
{
	entityTriggered = entity;
}

GameEntity* Player::GetTriggeringEntity()
{
	return entityTriggered;
}

void Player::SetHoldEntity(GameEntity* entity)
{
	isHolding = true;
	entityHeld = entity;
	MoveHeld();
	// Switch models here?
}

void Player::MoveHeld() {
	if (isHolding) {
		if (auto vegetable = dynamic_cast<Vegetable*>(entityHeld)) {
		
			const glm::vec4 vegetableLocation = glm::vec4(0, 0, -veggieHeldDist, 1) * GetRotation();
			const glm::mat4 held_rotation = glm::rotate(rotate.y, glm::vec3(0, 1, 0));
			vegetable->SetPosition(glm::vec3(translate->x + (vegetableLocation.x), vegetable->GetHeight(), (-1) * translate->y - (vegetableLocation.z)));
			vegetable->SetRotation(held_rotation);
		}
		else if (auto seed = dynamic_cast<Seed*>(entityHeld)) {

			const glm::vec4 seedLocation = glm::vec4(0, 0, -seedHeldDist, 1) * GetRotation();
			const glm::mat4 held_rotation = glm::rotate(rotate.y, glm::vec3(0, 1, 0));
			seed->SetPosition(glm::vec3(translate->x + (seedLocation.x), seed->GetHeight(), (-1) * translate->y - (seedLocation.z)));
			seed->SetRotation(held_rotation);
		}
		else if (auto garden_tool = dynamic_cast<GardenTool*>(entityHeld)) {
			const glm::vec4 location = glm::vec4(0, 0, -entityHeldDist, 1) * GetRotation();
			const glm::mat4 held_rotation = glm::rotate(rotate.y, glm::vec3(0, 1, 0));
			garden_tool->SetPosition(glm::vec3(translate->x + (location.x), garden_tool->GetHeight(), (-1) * translate->y - (location.z)));
			garden_tool->SetRotation(held_rotation);
		}
	}

}