#pragma once

#include "GameEntity.h"
#include "Drawable.h"

class Particle : public GameEntity, public Drawable {
	public:
		AniMode modelAnim = PARTICLE_STOP;

		Particle(ModelEnum curr);
		~Particle();

		// GameEntity
		void FixedUpdate() override;

		// Drawable
		glm::mat4 GetParentTransform() override;
		ModelEnum GetModelEnum() override;
		AniMode GetAniMode() override;

		// Information
		glm::vec3 GetPosition() const;
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::mat4 rotation);
		glm::mat4 GetRotation() const;
		float glowParticleHeight = -1.5;
		float dustParticleHeight = -3;
		float dustParticleZOffset = -3.f;

	private:
		// Transformations
		glm::vec2* translate = nullptr;
		glm::vec3* translate3D = nullptr;
		glm::mat4 rotation{};

		ModelEnum model; 

		// Get matrix transformation
		glm::mat4 GetTransformation();
		glm::mat4 GetTranslation();


};