#pragma once

#include "glm.h"
#include "ColliderCircle.h"
#include "Drawable.h"
#include "GameEntity.h"
#include "PhysicsObject.h"
#include "Interactable.h"
#include "util.h"
#include "Network/NetworkPacket.h"
#include "Particle.h"

class Player : public Drawable, public GameEntity, public PhysicsObject, public Interactable {
	private:
		// Transformations for our player
		glm::vec2* translate = nullptr;
		glm::vec3 rotate;
		glm::vec3 scale;

		// Player Movement Attributes
		float base_friction_ = 50.f; // Resistance in Units/Sec
		float drunk_friction_ = 5.f;
		float base_accel_ = 200.f; // Acceleration in Units/Sec^2
		float drunk_accel_ = 20.f;
		float max_eggplant_velocity_ = 10.f; // Eggplant speed
		float max_velocity_ = 20.f; // Max Velocity in Units/Sec
		float sprint_modifier = 1.5f;
		float entityHeldDist = 2.5f; // distance of entity from player
		float seedHeldDist = 2.5f; // distance of entity from player
		float veggieHeldDist = 3.f; // distance of entity from player
		glm::vec2 curr_vel_ = glm::vec2(0,0);

		// Current model to display
		ModelEnum model;

		// Currently holding
		GameEntity* entityHeld = nullptr;
		GameEntity* entityTriggered = nullptr;

		// Current Player Collider
		// Player will use a Circle Collider
		// ColliderCircle* collider_;
		std::vector<ColliderCircle*> colliders_;

		// Moving player
		void Move();
		void MoveHeld();
	
		// Get matrix transformationa
		glm::mat4 GetRotation();
		glm::mat4 GetTranslation();
		glm::mat4 GetScale();

		bool moveable = true;

	public:
		// current animation, no custom get/set logic so is set as public field
		AniMode modelAnim;

		Player();

		/* Curr should be idle */
		Player(ModelEnum curr);

		~Player() override;

		// GameEntity
		void FixedUpdate() override;

		// Drawable
		glm::mat4 GetParentTransform() override;
		ModelEnum GetModelEnum() override;
		AniMode GetAniMode() override;

		// The sound!!
		std::vector<SoundInfo> GetSounds();
		void ResetSoundBools();

		// PhysicsObject
		void OnCollide(PhysicsObject* object) override {}
		void OnTrigger(PhysicsObject* object) override;
		std::vector<Collider*> GetColliders() override;
		glm::vec2* GetWorldPosition() override;

		// Interactable
		bool CanInteract(Player* other_player) override;
		void OnInteract(Player* other_player) override;
		
		// Input
		glm::vec2 move_input{0,0};
		bool sprint = false;
		bool isDancing = false;
		bool ui_open = false;
		bool sale_confirm_ui_open = false; 
		void Use();
		void Drop();
		void Dance();
		void Buy(VegetableType bought_vegetable);
		void Buy(ModelEnum tool);
		void Sell();
		void CloseUI();
		void OpenUI();
		void OpenSaleUI();
		void CloseSaleUI();
		void EnableMovement();
		void DisableMovement();
		bool GetMoveable();
		void SetSprint(bool sprinting);


		// Information
		glm::vec3 GetPosition() const;
		void SetWorldPosition(glm::vec3 position);
		bool GetIsHolding() const { return isHolding; }
		GameEntity* GetTriggeringEntity();
		GameEntity* GetHoldEntity();
		void SetRotation(glm::vec3 newRot);

		void SetHoldEntity(GameEntity* entity);
		bool isHolding = false;
		void SetTriggeringEntity(GameEntity* entity); // Set the game object we're colliding with

		// how much money player currently has
		float curr_balance = 50;

		// particles
		Particle* dust_particle = nullptr;

		// stamina
		float baseStaminaIncreaseRate = 10;
		float oatStaminaIncreaseRate = 50;
		float baseStaminaDecreaseRate = 30;
		float oatStaminaDecreaseRate = 5;
		float curr_stamina = 100;
		// glue
		bool isGlued = false;

		// soju
		float intoxicationTimeRemaining = 0;
		float maxIntoxicationTime = 30;

		// oats
		float oatTimeRemaining = 0;
		float maxOatTime = 10;

		float playerHeight = 0;

		// Sound Attributes
		bool sound_buy = false;
		bool sound_sell = false;
		bool sound_plot_placement = false;
		bool sound_poison = false;
		bool sound_plant = false;
		bool sound_harvest = false;
		bool sound_eggplant_pickup = false;
		bool sound_steal = false;
		bool sound_shovel = false;
		bool sound_watering = false;
		bool sound_no = false;
		bool sound_fertilize = false;
		bool sound_net = false;
};