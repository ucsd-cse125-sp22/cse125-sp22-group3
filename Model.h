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
#include <stack>
#include <unordered_set>

#include "assimpConvert.h"
#include "stb_image.h"
#include "Mesh.h"
#include "util.h"

struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

struct AnimationNode
{
	bool hasBone;
	glm::mat4 transformation;
	glm::mat4 bone_trans;
	std::string name;
	int parent; 

	std::vector<KeyPosition> positions;
	std::vector<KeyRotation> rotations;
	std::vector<KeyScale> scales;

	int numPositions;
	int numRotations;
	int numScales;
};

class Model
{
private:
	// Texture + meshes
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;

	// Animation data
	std::map<std::string, BoneInfo> boneInfoMap;
	int boneCounter = 0;
	float blendOffset = 0.1f;
	bool hasAni = false;

	std::vector<std::vector<AnimationNode>> animationChannels;

	std::map<AniMode, int> animationMap = { 
		{IDLE_HOLD, 1},
		{IDLE_WALK, 2},
		{IDLE, 3},
		{WALK, 4},
		{NO_ANI, 5}
	};

	std::map<ModelEnum, std::string> modelFilePathMap = {
		{CHAR_BUMBUS, "models/bumbus/bumbus.fbx"},
		{CHAR_POGO, "models/pogo/pogo.fbx"},
		{CHAR_SWAINKY, "models/swainky/swainky.fbx"},
		{CHAR_GILMAN, "models/gilma/char4.fbx"}, // TODO rename

		{VEG_CABBAGE, "models/cabbage/cabbage.fbx"},
		{VEG_CARROT, "models/carrot/carrot.fbx"},
		{VEG_CORN, "models/corn/corn.fbx"}, 
		{VEG_RADISH, "models/raddish/raddish.fbx"},
		{VEG_TOMATO, "models/tomato/tomato.fbx"},
	};

	// Get previous and current blending
	AniMode curr;
	AniMode last;

	// Blend factor for blending animations
	float blend = 0.f;

	// Duration of each animation (agreed to be the same val if multiple)
	float duration = 0.f;

	// Delta time for animation
	glm::mat4 gt;
	glm::mat4 git;
	std::vector<glm::mat4> finalBoneMatrices;
	float ticks;

public:
	// Constructor + Deconstructor
	Model();

	Model(ModelEnum model);
	~Model();

	// Animation Speed
	float anim_speed = 1;

	// Rendering functions
	void draw(const glm::mat4& view, const glm::mat4& projection, glm::mat4 parent, GLuint shader);

	// Shadow map
	void draw(glm::mat4 parent, GLuint shader);

	// Loading meshes of model
	void processNode(aiNode * root, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory);

	// Load Animation Data
	void ExtractBoneWeightForVertices(std::vector<glm::ivec4>& bones, std::vector<glm::vec4>& weights, 
		aiMesh* mesh, const aiScene* scene);
	void SetVertexBoneData(std::vector<glm::ivec4>& bones, std::vector<glm::vec4>& weights,
		int vertexId, int boneID, float weight);
	void LoadAnimationData(const aiScene* scene);
	void CalculateBoneTransform(float time);

	void ReadHierarchyData(float time);
	void ReadBlendedHierarchyData(float time);
	glm::mat4 InterpolatePosition(float time, AnimationNode animationNode);
	glm::mat4 InterpolateScale(float time, AnimationNode animationNode);
	glm::mat4 InterpolateRotation(float time, AnimationNode animationNode);
	int GetPositionIndex(float time, AnimationNode animationNode);
	int GetScaleIndex(float time, AnimationNode animationNode);
	int GetRotationIndex(float time, AnimationNode animationNode);

	// Set or get animation data
	std::map<std::string, BoneInfo> getBoneMap() { return boneInfoMap; }
	int& getBoneCount() { return boneCounter; }
	void setAnimationMode(AniMode ani);

	// Operators
	Model& operator=(const Model& t);
};

#endif
