#ifndef _MESH_H_
#define _MESH_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

// store texture info
struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

// store bone info for animation
struct BoneInfo
{
	/*id is index in finalBoneMatrices*/
	int id;

	/*offset matrix transforms vertex from model space to bone space*/
	glm::mat4 offset;

};

class Mesh {
	private:
		// Mesh data
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<glm::uvec3> indices;
		std::vector<Texture> textures;

		// Animation
		std::vector<glm::vec3> Tangent;
		std::vector<glm::vec3> Bitangent;
		//bone indexes which will influence this vertex
		std::vector<glm::ivec4> boneIds;
		//weights from each bone
		std::vector<glm::vec4> weights;

		int ticksPerSecond = 25.0f;

		// Buffers + Attribute pointers
		GLuint VAO;
		GLuint VBO, VBO_N, VBO_UV, VBO_BI, VBO_W, EBO;

		// Transformations
		glm::mat4 model;

	public:
		// true if mesh has texture - false otherwise
		bool hasTexture;

		// Constructor + Deconstructors
		Mesh();

		Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, 
			std::vector<glm::vec2> uvs, std::vector<glm::uvec3> indices, 
			std::vector<Texture> textures, std::vector<glm::ivec4> b, std::vector<glm::vec4> w);
		~Mesh();

		// Render mesh
		void draw(glm::mat4 view, glm::mat4 projection, glm::mat4 mod, GLuint shader);
		void draw(glm::mat4 view, glm::mat4 projection, glm::mat4 mod, std::vector<glm::mat4> transforms, GLuint shader);
};

#endif
