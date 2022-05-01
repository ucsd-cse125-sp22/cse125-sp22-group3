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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>


class FBO {
	private:
		unsigned int rbo;
		unsigned int attachments[2];

	public:
		// size of frame buffers
		const unsigned int SHADOW_WIDTH = 1024;
		const unsigned int SHADOW_HEIGHT = 1024;
		GLuint depthFBO;
		// texture for depth map
		static unsigned int dm;

		// projection * view
		static glm::mat4 lightSpaceMatrix;

		// Bloom
		static unsigned int colorBuffers[2];
		static unsigned int pColorBuffers[2];
		static unsigned int sceneFBO;
		static unsigned int pingpongFBO[2];


		FBO();
		~FBO();
		FBO(float np, float fp);
		FBO(int width, int height);
		void draw();
		void draw(GLuint shader);
};

#endif
