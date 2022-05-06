#pragma once
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <OpenGL/gl3.h>
class Particles {

public:
	Particles();
	~Particles();
	float getRandNumber();
	

private:
	struct Particle {
		glm::vec2 Position;
		glm::vec2 Velocity;
		glm::vec4 StartCol, EndCol;
		float Rotation = 0.0f;
		float StartSize, EndSize;
		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;
		bool Active = false;


	};
	uint32_t ParticleIndex = 420; // Change alloc size if you want
	std::vector<Particle> ParticleAlloc;

	
};
