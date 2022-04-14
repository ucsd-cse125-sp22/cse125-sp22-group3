#include "Model.h"

Model::Model(std::string filePath) {
	// Set current animation mode
	last = IDLE;
	curr = IDLE;

	// Load model at file path
	scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
	
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
	if (scene->HasAnimations()) {
		if (scene->mAnimations[0]->mTicksPerSecond != 0.0)
		{
			ticks = scene->mAnimations[0]->mTicksPerSecond;
		}
		else
		{
			ticks = 25.0f;
		}
	}

	// Set initial transformation matrices
	model = glm::mat4(1.0f);

	std::cout << scene->mNumAnimations << std::endl;
	// Get directory from filepah to get materials
	directory = filePath.substr(0, filePath.find_last_of('/'));
	processNode(scene->mRootNode, scene);

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
	if (scene->HasAnimations()) {
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

void Model::CalculateBoneTransform(float time)
{
	float time_tick = time * ticks * anim_speed;
	// Get current frame
	float at = fmod(time_tick, scene->mAnimations[animationMap[curr]]->mDuration);

	/**
	 * Start at root, make transformation as you go down to children
	 */

	//ReadHierarchyData(at, scene->mRootNode, glm::mat4(1.0f));
	// If we are not transitioning, use one animation node
	if (curr == last) {
		ReadHierarchyData(at, scene->mRootNode, glm::mat4(1.0f));
	}

	// Else, blend
	else {
		ReadBlendedHierarchyData(at, scene->mRootNode, glm::mat4(1.0f));
	}

}

void Model::ReadHierarchyData(float time, const aiNode* node, glm::mat4 parentTransform) {
	std::string node_name(node->mName.data);

	const aiAnimation* animation = scene->mAnimations[animationMap[curr]];
	glm::mat4 node_transform = AssimpConvert::ConvertMatrixToGLMFormat(node->mTransformation);

	const aiNodeAnim* node_anim = findNodeAnim(animation, node_name);

	if (node_anim) {
		// get transformations at this time
		glm::mat4 scaling = InterpolateScale(time, node_anim);
		glm::mat4 rotation = InterpolateRotation(time, node_anim);
		glm::mat4 translation = InterpolatePosition(time, node_anim);

		node_transform = translation * rotation * scaling;
	}

	glm::mat4 global = parentTransform * node_transform;
	// find the bone associated with this node
	if (boneInfoMap.find(node_name) != boneInfoMap.end()) // true if node_name exist in bone_mapping
	{
		int bone_index = boneInfoMap[node_name].id;
		finalBoneMatrices[bone_index] = git * global * boneInfoMap[node_name].offset;
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ReadHierarchyData(time, node->mChildren[i], global);
	}
}

void Model::ReadBlendedHierarchyData(float time, const aiNode* node, glm::mat4 parentTransform) {
	// std::cout << blend << std::endl;
	if (blend <= 1.f) {
		std::string node_name(node->mName.data);

		const aiAnimation* thisAnimation = scene->mAnimations[animationMap[last]];
		glm::mat4 this_node_transform = AssimpConvert::ConvertMatrixToGLMFormat(node->mTransformation);

		const aiNodeAnim* this_node_anim = findNodeAnim(thisAnimation, node_name);

		const aiAnimation* nextAnimation = scene->mAnimations[animationMap[curr]];
		glm::mat4 next_node_transform = AssimpConvert::ConvertMatrixToGLMFormat(node->mTransformation);

		const aiNodeAnim* next_node_anim = findNodeAnim(nextAnimation, node_name);

		if (this_node_anim) {
			// get transformations at this time
			glm::mat4 scaling = InterpolateScale(time, this_node_anim);
			glm::mat4 rotation = InterpolateRotation(time, this_node_anim);
			glm::mat4 translation = InterpolatePosition(time, this_node_anim);

			this_node_transform = translation * rotation * scaling;
		}

		if (next_node_anim) {
			// get transformations at this time
			glm::mat4 scaling = InterpolateScale(time, next_node_anim);
			glm::mat4 rotation = InterpolateRotation(time, next_node_anim);
			glm::mat4 translation = InterpolatePosition(time, next_node_anim);

			next_node_transform = translation * rotation * scaling;
		}

		// Interpolate between both
		glm::quat thisInterpolatedBlend = glm::quat_cast(this_node_transform);
		glm::quat nextInterpolatedBlend = glm::quat_cast(next_node_transform);
		glm::quat blendedRotationTransform = glm::slerp(thisInterpolatedBlend, nextInterpolatedBlend, blend);
		glm::mat4 blendedTransform = glm::toMat4(blendedRotationTransform);
		blendedTransform[3] = glm::mix(this_node_transform[3], next_node_transform[3], blend);

		glm::mat4 global = parentTransform * blendedTransform;
		// find the bone associated with this node
		if (boneInfoMap.find(node_name) != boneInfoMap.end()) // true if node_name exist in bone_mapping
		{
			int bone_index = boneInfoMap[node_name].id;
			finalBoneMatrices[bone_index] = git * global * boneInfoMap[node_name].offset;
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ReadBlendedHierarchyData(time, node->mChildren[i], global);
		}

		blend += blendOffset;
	}

	else {
		// Reset vals
		last = curr;
		blend = 0.f;
	}
}

const aiNodeAnim* Model::findNodeAnim(const aiAnimation* animation, const std::string node_name)
{
	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		const aiNodeAnim* node_anim = animation->mChannels[i];
		if (std::string(node_anim->mNodeName.data) == node_name)
		{
			return node_anim;
		}
	}

	return nullptr;
}

glm::mat4 Model::InterpolatePosition(float time, const aiNodeAnim* animationNode) {
	if (animationNode->mNumPositionKeys == 1)
		return glm::translate(glm::mat4(1.0f), glm::vec3(animationNode->mPositionKeys[0].mValue.x, 
			animationNode->mPositionKeys[0].mValue.y, animationNode->mPositionKeys[0].mValue.z));

	int p0Index = GetPositionIndex(time, animationNode);
	int p1Index = p0Index + 1;

	float midWayLength = time - (float)animationNode->mPositionKeys[p0Index].mTime;
	float framesDiff = (float)(animationNode->mPositionKeys[p1Index].mTime - animationNode->mPositionKeys[p0Index].mTime);
	float scaleFactor = midWayLength / framesDiff;

	// std::cout << scaleFactor << std::endl;

	glm::vec3 begin(animationNode->mPositionKeys[p0Index].mValue.x,
		animationNode->mPositionKeys[p0Index].mValue.y, animationNode->mPositionKeys[p0Index].mValue.z);
	glm::vec3 end(animationNode->mPositionKeys[p1Index].mValue.x,
		animationNode->mPositionKeys[p1Index].mValue.y, animationNode->mPositionKeys[p1Index].mValue.z);

	glm::vec3 finalPosition = glm::mix(begin, end, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Model::InterpolateScale(float time, const aiNodeAnim* animationNode) {
	if (animationNode->mNumScalingKeys == 1)
		return glm::scale(glm::mat4(1.0f), glm::vec3(animationNode->mScalingKeys[0].mValue.x, 
			animationNode->mScalingKeys[0].mValue.y, animationNode->mScalingKeys[0].mValue.z));

	int p0Index = GetScaleIndex(time, animationNode);
	int p1Index = p0Index + 1;

	float midWayLength = time - (float)animationNode->mScalingKeys[p0Index].mTime;
	float framesDiff = (float)(animationNode->mScalingKeys[p1Index].mTime - animationNode->mScalingKeys[p0Index].mTime);
	float scaleFactor = midWayLength / framesDiff;

	glm::vec3 begin(animationNode->mScalingKeys[p0Index].mValue.x,
		animationNode->mScalingKeys[p0Index].mValue.y, animationNode->mScalingKeys[p0Index].mValue.z);
	glm::vec3 end(animationNode->mScalingKeys[p1Index].mValue.x,
		animationNode->mScalingKeys[p1Index].mValue.y, animationNode->mScalingKeys[p1Index].mValue.z);

	// Get interpolated scale
	glm::vec3 finalScale = glm::mix(begin, end, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}

glm::mat4 Model::InterpolateRotation(float time, const aiNodeAnim* animationNode) {
	if (animationNode->mNumRotationKeys == 1)
	{
		glm::quat rotation = glm::normalize(AssimpConvert::GetGLMQuat(animationNode->mRotationKeys[0].mValue));
		return glm::toMat4(rotation);
	}

	int p0Index = GetRotationIndex(time, animationNode);
	int p1Index = p0Index + 1;
	assert(p1Index < animationNode->mNumRotationKeys);

	float midWayLength = time - (float)(animationNode->mRotationKeys[p0Index].mTime);
	float framesDiff = (float) (animationNode->mRotationKeys[p1Index].mTime - animationNode->mRotationKeys[p0Index].mTime);
	float scaleFactor = midWayLength / framesDiff;

	glm::quat finalRotation = glm::slerp(AssimpConvert::GetGLMQuat(animationNode->mRotationKeys[p0Index].mValue),
		AssimpConvert::GetGLMQuat(animationNode->mRotationKeys[p1Index].mValue), scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}

int Model::GetPositionIndex(float time, const aiNodeAnim* animationNode) {
	for (int i = 0; i < animationNode->mNumPositionKeys - 1; i++)
	{
		if (time < (float)animationNode->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

int Model::GetScaleIndex(float time, const aiNodeAnim* animationNode) {
	for (int i = 0; i < animationNode->mNumScalingKeys - 1; i++)
	{
		if (time < (float)animationNode->mScalingKeys[i + 1].mTime)
			return i;
	}
	assert(0);
	return 0;
}

int Model::GetRotationIndex(float time, const aiNodeAnim* animationNode) {
	for (int i = 0; i < animationNode->mNumRotationKeys - 1; i++)
	{
		if (time < (float)animationNode->mRotationKeys[i + 1].mTime)
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