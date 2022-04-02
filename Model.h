#ifndef _MODEL_H_
#define _MODEL_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"
#include "Mesh.h"

class Model
{
private:
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;

	std::string directory;

public:
	// Constructor + Deconstructor
	Model();
	Model(std::string filePath);
	~Model();

	// Rendering functions
	void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader);
	void update();

	// Transformations
	void rotate(float deg, glm::vec3 rot);

	// Loading meshes of model
	void processNode(aiNode * root, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory);
};

#endif
