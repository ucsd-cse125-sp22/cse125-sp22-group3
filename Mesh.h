#ifndef _MESH_H_
#define _MESH_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


#include <iostream>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
	private:
		// Mesh data
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<glm::uvec3> indices;
		std::vector<Texture> textures;

		// Buffers + Attribute pointers
		GLuint VAO;
		GLuint VBO, VBO_N, VBO_UV, EBO;

		// Transformations
		glm::mat4 model;

	public:
		// true if mesh has texture - false otherwise
		bool hasTexture;

		// Constructor + Deconstructors
		Mesh();

		Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, 
				std::vector<glm::vec2> uvs, std::vector<glm::uvec3> indices, std::vector<Texture> textures);
		~Mesh();

		// set up pointers + buffers
		void setUpMesh();

		// Render mesh
		void draw(glm::mat4 view, glm::mat4 projection, GLuint shader);

		// Apply transformations
		void rotate(glm::mat4 rot);
};

#endif
