#include "Model.h"

Model::Model() {}

Model::Model(ModelEnum thisModel) {
	if (model_cache.count(thisModel) == 0) {
		constructorHelper(thisModel);
		model_cache[thisModel] = *this;
	}
	else {
		copyHelper(model_cache[thisModel]);
	}
}

Model::Model(const Model &other) {
	copyHelper(other);
}

void Model::constructorHelper(ModelEnum thisModel) {
	model = thisModel;
	// Set current animation mode
	last = IDLE;
	curr = IDLE;

	// Load model at file path
	Assimp::Importer importer;
	std::string filePath = modelFilePathMap[model];
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	// Potential errors opening model
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ASSIMP ERROR" << importer.GetErrorString() << std::endl;
		return;
	}

	// Extract global transforms + inverse
	gt = AssimpConvert::ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation);
	git = glm::inverse(gt);

	// Get ticks per scond if animation provides
	hasAni = scene->HasAnimations();
	if (scene->HasAnimations()) {
		if (scene->mAnimations[0]->mTicksPerSecond != 0.0)
		{
			ticks = scene->mAnimations[0]->mTicksPerSecond;
		}
		else
		{
			ticks = 25.0f;
		}

		animationChannels.resize(scene->mNumAnimations);

		duration = scene->mAnimations[1]->mDuration;
	}

	// Get directory from filepah to get materials
	directory = filePath.substr(0, filePath.find_last_of('/'));
	processNode(scene->mRootNode, scene);

	if (hasAni) { LoadAnimationData(scene); }

	/*
	std::cout << "scene->HasAnimations() 1: " << scene->HasAnimations() << std::endl;
	std::cout << "scene->mNumMeshes 1: " << scene->mNumMeshes << std::endl;
	std::cout << "scene->mAnimations[0]->mNumChannels 1: " << scene->mAnimations[0]->mNumChannels << std::endl;
	std::cout << "scene->mAnimations[0]->mDuration 1: " << scene->mAnimations[0]->mDuration << std::endl;
	std::cout << "scene->mAnimations[0]->mTicksPerSecond 1: " << scene->mAnimations[0]->mTicksPerSecond << std::endl << std::endl;
	*/
}

Model::~Model() {

}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// process meshes

	/*
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
	*/

	Mesh mesh;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* ai_mesh = scene->mMeshes[i];
		mesh = processMesh(ai_mesh, scene);
		meshes.push_back(mesh); //accumulate all meshes in one vector
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::cout << mesh->mName.C_Str() << std::endl;

	// Has materials
	bool hasMaterials = false;

	std::cout << "bones: " << mesh->mNumBones << " vertices: " << mesh->mNumVertices << std::endl;

	// Data to send to mesh
    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> norms;
    std::vector<glm::vec2> uvs;
    std::vector<glm::uvec3> indices;
	std::vector<Texture> textures;
	std::vector<glm::ivec4> boneIds;
	std::vector<glm::vec4> weights;

	// Get vertex data
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// set default animation info 
		boneIds.push_back(glm::ivec4(-1));
		weights.push_back(glm::vec4(0.f));

		// process vertex positions, normals and texture coordinates
		pos.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

		if (mesh->HasNormals()) {
			norms.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		}

		if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
			uvs.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
		}

		else {
			uvs.push_back(glm::vec2(0.f, 0.f));
		}
	}

	// Get indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector

		
		if (face.mNumIndices == 3) {

			indices.push_back(glm::uvec3(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
		}

		/*
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			std::cout << (face.mNumIndices) << std::endl;
			indices.push_back(face.mIndices[j]);
		}
		*/
	}
	
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// Get materials
	if (mesh->mMaterialIndex >= 0)
	{
		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}
	
	ExtractBoneWeightForVertices(boneIds, weights, mesh, scene);
	Mesh m = Mesh(pos, norms, uvs, indices, textures, boneIds, weights);

	return m;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		std::cout << "Texture loaded at path: " << path << std::endl;
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Model::draw(const glm::mat4& view, const glm::mat4& projection, glm::mat4 parent, GLuint shader) {
	if (hasAni) {
		float currTime = glfwGetTime();
	
		CalculateBoneTransform(currTime);

		for each (Mesh mesh in meshes)
		{
			mesh.draw(view, projection, parent, finalBoneMatrices, shader);
		}
		
	}

	else {
		for each (Mesh mesh in meshes)
		{
			mesh.draw(view, projection, parent, shader);
		}
	}
}

void Model::draw(glm::mat4 parent, GLuint shader) {
	if (hasAni) {
		float currTime = glfwGetTime();

		CalculateBoneTransform(currTime);

		for each (Mesh mesh in meshes)
		{
			mesh.draw(finalBoneMatrices, parent, shader);
		}

	}

	else {
		for each (Mesh mesh in meshes)
		{
			mesh.draw(parent, shader);
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<glm::ivec4>& vBones, std::vector<glm::vec4>& vWeights, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		std::cout << mesh->mBones[boneIndex]->mName.data << std::endl;

		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

		// Does this bone exist already? If not, add to the map 
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = boneCounter;
			newBoneInfo.offset = AssimpConvert::ConvertMatrixToGLMFormat(
				mesh->mBones[boneIndex]->mOffsetMatrix);

			// Store new bone in our map
			boneInfoMap[boneName] = newBoneInfo;
			boneID = boneCounter;

			// Increase number of bones found
			boneCounter++;
			finalBoneMatrices.push_back(glm::mat4(1.0f));
		}

		// It exist, retrieve the bone id
		else
		{
			boneID = boneInfoMap[boneName].id;
		}

		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vBones.size());
			SetVertexBoneData(vBones, vWeights, vertexId, boneID, weight);
		}
	}
}

void Model::SetVertexBoneData(std::vector<glm::ivec4>& bones, std::vector<glm::vec4>& weights,
	int vertexId, int boneID, float weight)
{
	for (int i = 0; i < 4; ++i)
	{
		if (weights[vertexId][i] == 0.f || bones[vertexId][i] == -1)
		{
			weights[vertexId][i] = weight;
			bones[vertexId][i] = boneID;
			break;
		}
	}
}

void Model::LoadAnimationData(const aiScene* scene) {
	// Temporary node to extract parents
	struct Node
	{
		const aiNode* src = nullptr;
		int parent = -1; // -1 = no parent, this is our root
	};

	std::vector<AnimationNode> aniNodes;
	std::map<std::string, int> node_names;
	std::stack<Node> getParents;

	getParents.push(Node{ scene->mRootNode, -1/*no parent*/ });

	// Get parent indices and transformations
	while (getParents.size() > 0) {
		Node data = std::move(getParents.top());
		getParents.pop();

		// Load into animation node
		AnimationNode node;
		node.parent = data.parent;
		node.transformation = AssimpConvert::ConvertMatrixToGLMFormat(data.src->mTransformation);
		node.hasBone = false;
		node.name = std::string(data.src->mName.data);
		assert(node.parent < int(aniNodes.size()));
		aniNodes.push_back(node);
		node_names.insert(std::pair<std::string, int>(data.src->mName.data, 
			aniNodes.size() - 1));
		const int parent_index = int(aniNodes.size() - 1);

		for (unsigned i = 0; i < data.src->mNumChildren; ++i)
		{
			getParents.push(Node{ data.src->mChildren[i], parent_index });
		}
	}

	// For every animation type. Skip the one where all is combined (i = 0)
	for (int i = 1; i < scene->mNumAnimations; i++) {
		// Copy vector for every animation
		std::vector<AnimationNode> nodes = aniNodes;
		animationChannels[i] = nodes;
		const aiAnimation* animation = scene->mAnimations[i];
		for (int j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
		{
			auto channel = animation->mChannels[j];
			std::string name = std::string(channel->mNodeName.data);

			std::vector<KeyPosition> positionKeys;
			std::vector<KeyRotation> rotationKeys;
			std::vector<KeyScale> scaleKeys;

			if (node_names.find(name) != node_names.end()) // true if node_name exist
			{
				AnimationNode thisNode = aniNodes[node_names[name]];
				thisNode.hasBone = true;

				// Add key vals for animation
				thisNode.numPositions = channel->mNumPositionKeys;

				for (int positionIndex = 0; positionIndex < thisNode.numPositions; ++positionIndex)
				{
					aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
					float timeStamp = channel->mPositionKeys[positionIndex].mTime;
					KeyPosition data;
					data.position = AssimpConvert::GetGLMVec(aiPosition);
					data.timeStamp = timeStamp;
					positionKeys.push_back(data);
				}

				thisNode.numRotations = channel->mNumRotationKeys;
				for (int rotationIndex = 0; rotationIndex < thisNode.numRotations; ++rotationIndex)
				{
					aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
					float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
					KeyRotation data;
					data.orientation = AssimpConvert::GetGLMQuat(aiOrientation);
					data.timeStamp = timeStamp;
					rotationKeys.push_back(data);
				}

				thisNode.numScales = channel->mNumScalingKeys;
				for (int keyIndex = 0; keyIndex < thisNode.numScales; ++keyIndex)
				{
					aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
					float timeStamp = channel->mScalingKeys[keyIndex].mTime;
					KeyScale data;
					data.scale = AssimpConvert::GetGLMVec(scale);
					data.timeStamp = timeStamp;
					scaleKeys.push_back(data);
				}

				// Add to node
				thisNode.positions = positionKeys;
				thisNode.rotations = rotationKeys;
				thisNode.scales = scaleKeys;
				this->animationChannels[i][node_names[name]] = thisNode;
			}
		}
	}
}

void Model::CalculateBoneTransform(float time)
{
	float time_tick = time * ticks * anim_speed;
	// Get current frame
	float at = fmod(time_tick, duration);

	/**
	 * Start at root, make transformation as you go down to children
	 */

	//ReadHierarchyData(at, scene->mRootNode, glm::mat4(1.0f));
	// If we are not transitioning, use one animation node
	if (curr == last) {
		ReadHierarchyData(at);
	}

	// Else, blend
	else {
		ReadBlendedHierarchyData(at);
	}

}

void Model::ReadHierarchyData(float time) {
	for (int i = 0; i < animationChannels[animationMap[curr]].size(); i++) {
		AnimationNode& node = animationChannels[animationMap[curr]][i];

		glm::mat4 node_trans = node.transformation;
		// get transformations at this time
		if (node.hasBone) {
			glm::mat4 scaling = InterpolateScale(time, node);
			glm::mat4 rotation = InterpolateRotation(time, node);
			glm::mat4 translation = InterpolatePosition(time, node);
			node_trans = translation * rotation * scaling;
		}

		// get parent
		glm::mat4 global = glm::mat4(1.0f);
		if (node.parent != -1) {
			AnimationNode& parent = animationChannels[animationMap[curr]][node.parent];
			global = parent.bone_trans;
		}

		global = global * node_trans;
		node.bone_trans = global;

		if (boneInfoMap.find(node.name) != boneInfoMap.end()) // true if node_name exist in bone_mapping
		{
			int bone_index = boneInfoMap[node.name].id;
			finalBoneMatrices[bone_index] = git * global * boneInfoMap[node.name].offset;
		}
	}
}

void Model::ReadBlendedHierarchyData(float time) {
	if (blend <= 1.f) {
		for (int i = 0; i < animationChannels[animationMap[curr]].size(); i++) {
			AnimationNode& node = animationChannels[animationMap[last]][i];
			assert(int(i) > node.parent);

			glm::mat4 this_node_transform = node.transformation;
			if (node.hasBone) {
				// get transformations at this time
				glm::mat4 this_scaling = InterpolateScale(time, node);
				glm::mat4 this_rotation = InterpolateRotation(time, node);
				glm::mat4 this_translation = InterpolatePosition(time, node);

				this_node_transform = this_translation * this_rotation * this_scaling;
			}

			AnimationNode& next_node = animationChannels[animationMap[curr]][i];
			glm::mat4 next_node_transform = next_node.transformation;
			if (next_node.hasBone) {
				// get transformations at next time
				glm::mat4 next_scaling = InterpolateScale(time, next_node);
				glm::mat4 next_rotation = InterpolateRotation(time, next_node);
				glm::mat4 next_translation = InterpolatePosition(time, next_node);

				next_node_transform = next_translation * next_rotation * next_scaling;
			}

			// get parent
			glm::mat4 global = glm::mat4(1.0f);
			if (node.parent != -1) {
				AnimationNode& parent = animationChannels[animationMap[last]][node.parent];
				global = parent.bone_trans;
			}

			// Interpolate between both
			glm::quat thisInterpolatedBlend = glm::quat_cast(this_node_transform);
			glm::quat nextInterpolatedBlend = glm::quat_cast(next_node_transform);
			glm::quat blendedRotationTransform = glm::slerp(thisInterpolatedBlend, nextInterpolatedBlend, blend);
			glm::mat4 blendedTransform = glm::toMat4(blendedRotationTransform);
			blendedTransform[3] = glm::mix(this_node_transform[3], next_node_transform[3], blend);

			global = global * blendedTransform;
			node.bone_trans = global;
			next_node.bone_trans = global;

			if (boneInfoMap.find(node.name) != boneInfoMap.end()) // true if node_name exist in bone_mapping
			{
				int bone_index = boneInfoMap[node.name].id;
				finalBoneMatrices[bone_index] = git * global * boneInfoMap[node.name].offset;
			}
		}
		blend += blendOffset;
	}

	else {
		// Reset vals
		last = curr;
		blend = 0.f;
	}
}

glm::mat4 Model::InterpolatePosition(float time, AnimationNode node) {
	if (node.numPositions == 1)
		return glm::translate(glm::mat4(1.0f), node.positions[0].position);

	int p0Index = GetPositionIndex(time, node);
	int p1Index = p0Index + 1;

	float midWayLength = time - (float)node.positions[p0Index].timeStamp;
	float framesDiff = (float)(node.positions[p1Index].timeStamp - node.positions[p0Index].timeStamp);
	float scaleFactor = midWayLength / framesDiff;

	// std::cout << scaleFactor << std::endl;

	glm::vec3 begin = node.positions[p0Index].position;
	glm::vec3 end = node.positions[p1Index].position;

	glm::vec3 finalPosition = glm::mix(begin, end, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Model::InterpolateScale(float time, AnimationNode node) {
	if (node.numScales == 1)
		return glm::scale(glm::mat4(1.0f), node.scales[0].scale);

	int p0Index = GetScaleIndex(time, node);
	int p1Index = p0Index + 1;

	float midWayLength = time - (float)node.scales[p0Index].timeStamp;
	float framesDiff = (float)(node.scales[p1Index].timeStamp - node.scales[p0Index].timeStamp);
	float scaleFactor = midWayLength / framesDiff;

	glm::vec3 begin = node.scales[p0Index].scale;
	glm::vec3 end = node.scales[p1Index].scale;

	// Get interpolated scale
	glm::vec3 finalScale = glm::mix(begin, end, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}

glm::mat4 Model::InterpolateRotation(float time, AnimationNode node) {
	if (node.numRotations == 1)
	{
		glm::quat rotation = glm::normalize(node.rotations[0].orientation);
		return glm::toMat4(rotation);
	}

	int p0Index = GetRotationIndex(time, node);
	int p1Index = p0Index + 1;
	assert(p1Index < node.numRotations);

	float midWayLength = time - (float)(node.rotations[p0Index].timeStamp);
	float framesDiff = (float) (node.rotations[p1Index].timeStamp - node.rotations[p0Index].timeStamp);
	float scaleFactor = midWayLength / framesDiff;

	glm::quat finalRotation = glm::slerp(node.rotations[p0Index].orientation,
		node.rotations[p1Index].orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}

int Model::GetPositionIndex(float time, AnimationNode animationNode) {
	for (int i = 0; i < animationNode.numPositions - 1; i++)
	{
		if (time < (float)animationNode.positions[i + 1].timeStamp)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

int Model::GetScaleIndex(float time, AnimationNode animationNode) {
	for (int i = 0; i < animationNode.numScales - 1; i++)
	{
		if (time < (float)animationNode.scales[i + 1].timeStamp)
			return i;
	}
	assert(0);
	return 0;
}

int Model::GetRotationIndex(float time, AnimationNode animationNode) {
	for (int i = 0; i < animationNode.numRotations - 1; i++)
	{
		if (time < (float)animationNode.rotations[i + 1].timeStamp)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

void Model::setAnimationMode(AniMode ani) {
	// If we are not in the process of blending anything
	if (ani != curr || blend == 0.0f) {
		last = curr;
		curr = ani;
	}
}

void Model::copyHelper(const Model& t) {
	// Copy all values
	curr = t.curr;
	last = t.last;

	textures_loaded = t.textures_loaded;
	meshes = t.meshes;
	directory = t.directory;

	boneInfoMap = t.boneInfoMap;
	boneCounter = t.boneCounter;
	blendOffset = t.blendOffset;
	hasAni = t.hasAni;
	animationChannels = t.animationChannels;
	animationMap = t.animationMap;

	blend = t.blend;
	duration = t.duration;
	gt = t.gt;
	git = t.git;

	finalBoneMatrices = t.finalBoneMatrices;
	ticks = t.ticks;
}

Model& Model::operator=(const Model& t)
{
	copyHelper(t);

	return *this;
}
