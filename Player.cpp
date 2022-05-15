#include "Player.h"
#include "Hoe.h"
#include "GameManager.h"
#include "Network/NetworkPacket.h"

Player::Player() {
	translate = new glm::vec2(0.f,0.f);
	rotate = glm::vec3(0.0f);
	scale = glm::vec3(0.0f);

	collider_ = new ColliderCircle(glm::vec2(0,0), 3, false);
	dust_particle = new Particle(ModelEnum::PARTICLE_DUST);
	auto particle_ = dynamic_cast<Particle*>(dust_particle);
	particle_->modelAnim = PARTICLE_STOP;
	particle_->SetPosition(glm::vec3(( * translate)[0], particle_->dustParticleHeight, -(*translate)[1]));
	GameManager::AddEntities({ dust_particle });
}

GameEntity* Player::GetHoldEntity()
{
	return entityHeld;
}

Player::Player(ModelEnum curr) : Player() {
	// Set initial values
	model = curr;
	//std::cout << model.getBoneCount() << std::endl;
}

void Player::FixedUpdate() {

	if (glm::length(move_input) > 1) move_input = glm::normalize(move_input); 
	const auto delta = static_cast<float>(GameManager::GetFixedDeltaTime());
	// If no movement is given apply friction (epsilon to account for FP errors)
	if (glm::length(move_input) <= 0 || isGlued) {
		// increment stamina
		stamina_percent = fmin(stamina_percent + (delta*staminaIncreaseRate), 100);
		if (glm::length(curr_vel_) < friction_ * delta) curr_vel_ = glm::vec2(0,0);
		else {
			curr_vel_ -= glm::normalize(curr_vel_) * friction_ * delta;
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
		// increment stamina
		if(sprint)
			stamina_percent = fmax(stamina_percent - (delta*staminaDecreaseRate), 0);
		else {
			stamina_percent = fmin(stamina_percent + (delta * staminaIncreaseRate), 100);
		}

		// check if enough stamina to run
		if (stamina_percent == 0) {
			auto particle_ = dynamic_cast<Particle*>(dust_particle);
			particle_->modelAnim = PARTICLE_STOP;
			sprint = false;
		}

		// Accelerate in our inputted direction
		// Transform input from 2D to 3D Plane
		curr_vel_ += base_accel_ * move_input * delta;
		// Cap our speed at some max velocity
		const float max_vel = sprint ? max_sprint_velocity_ : max_velocity_;
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

	collider_->center = *translate;
	// move dust particle
	auto particle_ = dynamic_cast<Particle*>(dust_particle);
	particle_->SetPosition(glm::vec3((*translate)[0], particle_->dustParticleHeight, -(*translate)[1]));
	//particle_->SetRotation(GetParentTransform());
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

	return output;
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
	return { collider_ };
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
		glm::vec3 plotPosition = glm::vec3((*translate)[0], -4, -(*translate)[1]) + direction;
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
		glm::vec3 gluePosition = glm::vec3((*translate)[0], -4, -(*translate)[1]) + direction;
		glueOnGround->SetPosition(gluePosition);
		GameManager::AddEntities({ glueOnGround });
		glueOnGround->ownerOfGlue = this; // set owner
		Drop();
		GameManager::RemoveEntities({ glue });
	}
	else {
		auto entity = dynamic_cast<PhysicsObject*>(entityTriggered);
		if (entity != nullptr && collider_->CollidesWith(entity->GetColliders()[0])) {
			auto interactable = dynamic_cast<Interactable*>(entityTriggered);
			if (interactable != nullptr && interactable->CanInteract(this)) {
				interactable->OnInteract(this);
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
		VeggieInfo veggie_info = veggie_map[bought_vegetable];
		Seed* bought_seed = new Seed{ bought_vegetable, veggie_info.seed_model };
		GameManager::AddEntities({ bought_seed });
		SetHoldEntity(bought_seed);

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
}

void Player::SetSprint(bool sprinting) {
	
	// first time starting to sprint setting particles
	if (!sprint && sprinting) {

		auto particle_ = dynamic_cast<Particle*>(dust_particle);
		particle_->modelAnim = PARTICLE_PLAY;
	}
	// ending sprint
	else if (sprint && !sprinting) {
		auto particle_ = dynamic_cast<Particle*>(dust_particle);
		particle_->modelAnim = PARTICLE_STOP;
	}
	
	sprint = sprinting;

}

void Player::CloseUI()
{
	ui_open = false;
	moveable = true;
}

void Player::OpenUI()
{
	ui_open = true;
	moveable = false;
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
	return glm::translate(glm::vec3((* translate)[0], 0, -(*translate)[1]));
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