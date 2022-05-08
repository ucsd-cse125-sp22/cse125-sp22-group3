#include "GameEntity.h"
#include "Drawable.h"

class Particle : public GameEntity, public Drawable {
	public:
		AniMode modelAnim = NO_ANI;

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

	private:
		// Transformations
		glm::vec2* translate = nullptr;
		glm::mat4 rotation{};

		ModelEnum model; 

		// Get matrix transformation
		glm::mat4 GetTransformation();
		glm::mat4 GetTranslation();
};