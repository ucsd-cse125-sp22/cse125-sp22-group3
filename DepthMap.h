#ifndef _DEPTH_MAP_H_
#define _DEPTH_MAP_H_

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

class DepthMap {
	private:

	public:
		// size of frame buffers
		const unsigned int SHADOW_WIDTH = 1024;
		const unsigned int SHADOW_HEIGHT = 1024;	
		GLuint FBO;
		// texture for depth map
		static unsigned int dm;

		// projection * view
		static glm::mat4 lightSpaceMatrix;

		DepthMap();
		DepthMap(float np, float fp);
		// void draw(GameManager * gm, GLuint shader);
		void draw();
		// void draw(GameManager * gm, Model * m, GLuint shader);
};

#endif

