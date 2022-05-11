#ifndef _FBO_H_
#define _FBO_H_


#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include <vector>
class FBO {
	private:
		unsigned int rbo;
		unsigned int attachments[2];

		float near, far;

		glm::mat4 setLightSpaceMatrix(float np, float fp, glm::mat4 view, int width, int height);

	public:
		// size of frame buffers
		const unsigned int SHADOW_WIDTH = 4096;
		const unsigned int SHADOW_HEIGHT = 4096;
		GLuint depthFBO;

		// shadows
		static unsigned int dm;
		static unsigned int matricesUBO;

		// projection * view
		static std::vector<glm::mat4> lightSpaceMatrix;
		static std::vector<float> shadowCascadeLevels;

		static glm::vec3 lightPos;

		// Bloom
		static unsigned int colorBuffers[2];
		static unsigned int pColorBuffers[2];
		static unsigned int sceneFBO;
		static unsigned int pingpongFBO[2];


		FBO();
		~FBO();
		FBO(float np, float fp);
		FBO(int width, int height);
		void resize(int width, int height);
		void draw(int width, int height, glm::mat4 view);
		void draw(GLuint shader);
};

#endif
