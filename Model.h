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

#include <map>

#include "assimpConvert.h"
#include "stb_image.h"
#include "Mesh.h"

enum AniMode {
	IDLE_HOLD,
	IDLE_WALK,
	IDLE,
	WALK,
};

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Model
{
private:
	// Importing scene information
	Assimp::Importer importer;
	const aiScene* scene = nullptr;

	// Texture + meshes
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;

	// Bone information for animation
	std::map<std::string, BoneInfo> boneInfoMap;
	int boneCounter = 0;

	std::map<AniMode, int> animationMap = { 
		{IDLE_HOLD, 1},
		{IDLE_WALK, 2},
		{IDLE, 3},
		{WALK, 4},
	};

	AniMode curr;
	AniMode last;

	// Holds transformations for this model
	glm::mat4 model;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;

	// Delta time for animation
	glm::mat4 gt;
	glm::mat4 git;
	std::vector<glm::mat4> finalBoneMatrices;
	float ticks;

public:
	// Constructor + Deconstructor
	Model();
	Model(std::string filePath);
	~Model();

	// Animation Speed
	float anim_speed = 1;

	// Rendering functions
	void draw(const glm::mat4& view, const glm::mat4& projection, glm::mat4 parent, GLuint shader);

	// Loading meshes of model
	void processNode(aiNode * root, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory);

	// Animation
	void ExtractBoneWeightForVertices(std::vector<glm::ivec4>& bones, std::vector<glm::vec4>& weights, 
		aiMesh* mesh, const aiScene* scene);
	void SetVertexBoneData(std::vector<glm::ivec4>& bones, std::vector<glm::vec4>& weights,
		int vertexId, int boneID, float weight);
	void CalculateBoneTransform(float time);
	void ReadHierarchyData(float time, const aiNode* node, glm::mat4 parentTransform);
	const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string node_name);
	glm::mat4 InterpolatePosition(float time, const aiNodeAnim* animationNode);
	glm::mat4 InterpolateScale(float time, const aiNodeAnim* animationNode);
	glm::mat4 InterpolateRotation(float time, const aiNodeAnim* animationNode);
	int GetPositionIndex(float time, const aiNodeAnim * animationNode);
	int GetScaleIndex(float time, const aiNodeAnim* animationNode);
	int GetRotationIndex(float time, const aiNodeAnim* animationNode);

	// Set or get animation data
	std::map<std::string, BoneInfo> getBoneMap() { return boneInfoMap; }
	int& getBoneCount() { return boneCounter; }
	void setAnimationMode(AniMode ani);
};

#endif
