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

	// map to current animation
	std::map<AniMode, int> animationMap = { 
		{IDLE_HOLD, 1},
		{IDLE_WALK, 2},
		{IDLE, 3},
		{WALK, 4},
		{NO_ANI, 5}
	};

	// map to model file path
	std::map<ModelEnum, std::string> modelFilePathMap = {
		{CHAR_BUMBUS, "models/bumbus/bumbus.fbx"},
		{CHAR_POGO, "models/pogo/pogo.fbx"},
		{CHAR_SWAINKY, "models/swainky/swainky.fbx"},
		{CHAR_GILMAN, "models/gilma/char4.fbx"}, // TODO rename

		{VEG_CABBAGE, "models/cabbage/cabbage.fbx"},
		{VEG_CARROT, "models/carrot/carrot.fbx"},
		{VEG_CORN, "models/corn/corn.fbx"}, 
		{VEG_RADISH, "models/radish/radish.fbx"},
		{VEG_TOMATO, "models/tomato/tomato.fbx"},

		{WORLD_PLOT_RED, "models/plots/plotRed.fbx"},
		{WORLD_PLOT_BLUE, "models/plots/plotBlue.fbx"},
		{WORLD_PLOT_GREEN, "models/plots/plotGreen.fbx"},
		{WORLD_PLOT_YELLOW, "models/plots/plotYellow.fbx"},

		{WORLD_SEED_CABBAGE, "models/seeds/cabbageSeed.fbx"},
		{WORLD_SEED_CARROT, "models/seeds/carrotSeed.fbx"},
		{WORLD_SEED_CORN, "models/seeds/cornSeed.fbx"},
		{WORLD_SEED_RADISH, "models/seeds/radishSeed.fbx"},
		{WORLD_SEED_TOMATO, "models/seeds/tomatoSeed.fbx"},

		{WORLD_FLAG_CABBAGE, "models/plots/flagCabbage.fbx"},
		{WORLD_FLAG_CARROT, "models/plots/flagCarrot.fbx"},
		{WORLD_FLAG_CORN, "models/plots/flagCorn.fbx"},
		{WORLD_FLAG_RADISH, "models/plots/flagRadish.fbx"},
		{WORLD_FLAG_TOMATO, "models/plots/flagTomato.fbx"},

		{WORLD_MAP, "models/world/world.fbx"},
	};

	// map to texture (for models who share same mesh, but different textures)
	std::vector<std::string> veggieTextures{
		{"cabbage.png", "carrot.png", "corn.png", "raddish.png", "tomato.png"},
	};

	std::vector<std::string> plotColorTextures{
		"plotBlue.png","plotYellow.png","plotRed.png","plotGreen.png"
	};

	// Get previous and current blending
	AniMode curr;
	AniMode last;

	ModelEnum model;

	// Blend factor for blending animations
	float blend = 0.f;

	// Duration of each animation (agreed to be the same val if multiple)
	float duration = 0.f;

	// Delta time for animation
	glm::mat4 gt;
	glm::mat4 git;
	std::vector<glm::mat4> finalBoneMatrices;
	float ticks;

	void Model::constructorHelper(ModelEnum thisModel);

	std::map<ModelEnum, Model> model_cache;
	

public:
	// Constructor + Deconstructor
	Model();

	Model(ModelEnum thisModel);
	~Model();

	Model::Model(const Model &other);

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
	ModelEnum getModelEnum();
	//bool modelChanged = true;

	// Operators
	Model& operator=(const Model& t);
	void copyHelper(const Model& t);

	
};

#endif
