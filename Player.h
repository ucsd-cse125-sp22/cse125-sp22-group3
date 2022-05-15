#pragma once

#include "glm.h"
#include "ColliderCircle.h"
#include "Drawable.h"
#include "GameEntity.h"
#include "PhysicsObject.h"
#include "SoundEngine.h"
#include "util.h"
#include "Network/NetworkPacket.h"


class Player : public Drawable, public GameEntity, public PhysicsObject {
	private:
		// Transformations for our player
		glm::vec2* translate = nullptr;
		glm::vec3 rotate;
		glm::vec3 scale;

		// Player Movement Attributes
		float friction_ = 50.f; // Resistance in Units/Sec
		float base_accel_ = 200.f; // Acceleration in Units/Sec^2
		float max_velocity_ = 20.f; // Max Velocity in Units/Sec
		float max_sprint_velocity_ = 30.f;
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
		ColliderCircle* collider_;

		// Moving player
		void Move();
		void MoveHeld();

		// Sound Attributes
		bool sound_buy = false;
		bool sound_sell = false;
		bool sound_plot_placement = false;
	
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
		
		// Input
		glm::vec2 move_input{0,0};
		bool sprint = false;
		bool isDancing = false;
		bool ui_open = false;
		void Use();
		void Drop();
		void Dance();
		void Buy(VegetableType bought_vegetable);
		void Sell();
		void CloseUI();
		void OpenUI();
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

		void SetHoldEntity(GameEntity* entity);
		bool isHolding = false;
		void SetTriggeringEntity(GameEntity* entity); // Set the game object we're colliding with

		// how much money player currently has
		float curr_balance = 20;

		GameEntity* dust_particle;
		
		
};