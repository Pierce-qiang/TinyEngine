#include "Model.h"
#include <iostream>
#include "../texture/TextureLoader.h"
#include <assimp/postprocess.h>
#include "AssimpGLMHelpers.h"
namespace TEngine {
	unsigned int Model::noNameNum = 0;

	Model::Model(const std::string& path, bool hasBone /*= false*/):hasBone(hasBone)
	{
		LoadModel(path);
	}

	Model::Model(const Mesh& mesh)
	{
		mMeshes.push_back(mesh);
		mName = "None_";
		mName += std::to_string(noNameNum);
		noNameNum++;
	}

	Model::Model(const std::vector<Mesh>& meshes)
	{
		mMeshes = meshes;
		mName = "None_";
		mName += std::to_string(noNameNum);
		noNameNum++;
	}

	Model::~Model()
	{

	}

	void Model::Init()
	{
	}

	void Model::Draw(Shader* shader, bool isUseMaterial) const
	{
		for (unsigned int i = 0; i < mMeshes.size(); ++i)
		{
			if (isUseMaterial)
			{
				mMeshes[i].mMaterial.BindMaterial(shader);

				// check whether to use normal mapping
				if (mMeshes[i].HasTangents())
					shader->SetUniform("useNormalMapping", 1);
				else
					shader->SetUniform("useNormalMapping", 0);
			}



			mMeshes[i].Draw();
		}
	}

	void Model::LoadModel(const std::string& path)
	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString();
			return;
		}

		mDirectory = path.substr(0, path.find_last_of('/'));
		mName = mDirectory.substr(mDirectory.find_last_of('/')+1, mDirectory.size());

		ProcessNode(scene->mRootNode, scene);
	}

	// An alternative way is to process mesh from scene node directly in a simple loop.
	// But this recursive way can provide a parent-child relationship.
	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		// process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// each node has an array of mesh indices, use these indices to get the meshes from the scene
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			mMeshes.push_back(ProcessMesh(mesh, scene));
		}

		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		// prepare vertex memeory
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<unsigned int> indices;

		positions.reserve(mesh->mNumVertices);
		uvs.reserve(mesh->mNumVertices);
		normals.reserve(mesh->mNumVertices);
		tangents.reserve(mesh->mNumVertices);
		bitangents.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces * 3);


		// process each vertex
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			glm::vec2 textureCoord;
			// texture Coordinates (check if there is texture coordinates)
			if (mesh->mTextureCoords[0])
			{
				// A vertex can contain up to 8 different texture coordinates. We are just going to use the first one
				textureCoord.x = mesh->mTextureCoords[0][i].x;
				textureCoord.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				textureCoord.x = 0.0f;
				textureCoord.y = 0.0f;
			}

			positions.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
			uvs.push_back(glm::vec2(textureCoord.x, textureCoord.y));
			normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
			tangents.push_back(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
			bitangents.push_back(glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z));
		}

		// process indices:
		// Loop through every face (triangle thanks to aiProcess_Triangulate) and stores its indices in our meshes indices. 
		// This will ensure they are in the right order.
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		Mesh newMesh(positions, uvs, normals, tangents, bitangents, indices);
		if(hasBone) ExtractBoneWeights(newMesh, mesh);
		newMesh.SetupMesh();

		// process textures
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			// This set is only work for my sponza mtl file. be careful!
			newMesh.mMaterial.SetAlbedoMap(LoadMaterialTexture(material, aiTextureType_DIFFUSE, true));
			newMesh.mMaterial.SetNormalMap(LoadMaterialTexture(material, aiTextureType_NORMALS, false));
			newMesh.mMaterial.SetAmbientOcclusionMap(LoadMaterialTexture(material, aiTextureType_AMBIENT, false));
			newMesh.mMaterial.SetMetallicMap(LoadMaterialTexture(material, aiTextureType_SPECULAR, false));
			newMesh.mMaterial.SetRoughnessMap(LoadMaterialTexture(material, aiTextureType_SHININESS, false));
			newMesh.mMaterial.SetMixtureMap(LoadMaterialTexture(material, aiTextureType_UNKNOWN, false));

			if (material->GetTextureCount(aiTextureType_UNKNOWN) > 0)
				newMesh.mMaterial.SeperateMixture(); // try to seperate mixture for gltf file only;
		}

		return newMesh;
	}

	// TODO: It's better to cache the loaded texture to avoid mutiple load the same texture.
	Texture* Model::LoadMaterialTexture(aiMaterial* mat, aiTextureType type, bool isSRGB)
	{
		// Log material constraints are being violated (1 texture per type for the standard shader)
		if (mat->GetTextureCount(type) > 2)
		{
			// There is some strange phenomenon with gltf file, the same diffuse path will read two times,
			// so I set the threshold to 2 here.
			std::cout << "Mesh's default material contains more than 1 texture for the same type, which currently isn't supported by the standard shader\n";

			//std::cout << "Texture number:" << mat->GetTextureCount(type) << "\n";
			//aiString str;
			//mat->GetTexture(type, 0, &str);
			//std::cout << str.C_Str() << "\n";
			//aiString str2;
			//mat->GetTexture(type,1, &str2);
			//std::cout << str2.C_Str() << "\n";
		}

		// Load the texture of a certain type, assuming there is one
		if (mat->GetTextureCount(type) > 0)
		{
			aiString str;
			mat->GetTexture(type, 0, &str);

			// Assumption made: material stuff is located in the same directory as the model object
			// and the texture str is relative path, so we can concatenate with mDirectory
			std::string fileToSearch = (mDirectory + "/" + std::string(str.C_Str())).c_str();

			TextureSettings textureSettings;
			textureSettings.IsSRGB = isSRGB;

			for (auto& c : fileToSearch)
				if (c == '\\') c = '/';
			return TextureLoader::Load2DTexture(fileToSearch, &textureSettings);
		}

		return nullptr;
	}

	void Model::OnGui() {
		//ImGui::Text(mDirectory.c_str());

		ImGui::Text((std::string("Center: (") + std::to_string(mCenter.x) + ", " + std::to_string(mCenter.y) + ", " + std::to_string(mCenter.z) + " )").c_str());
		ImGui::DragFloat3("Pos", &mPosition[0], DRAG_SPEED);
		ImGui::DragFloat3("Scale", &mScale[0], DRAG_SPEED);
		ImGui::DragFloat("Rotate_Angle", &mRotation, DRAG_SPEED*10, -180, 180);
		ImGui::DragFloat3("RotateAxis", &mRotateAxis[0], DRAG_SPEED,-1,1);
		if (ImGui::TreeNode("Meshes"))
		{
			for (int i = 0; i < mMeshes.size(); i++) {
				if (ImGui::TreeNode((std::string("mesh_") + std::to_string(i)).c_str()))
				{
					mMeshes[i].OnGui();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		
		
	}

	//Refer to learnopengl
	void Model::ExtractBoneWeights(Mesh& newMesh, aiMesh* mesh) {
		//init Bones
		newMesh.mBoneIDs.reserve(newMesh.mPositions.size());
		newMesh.mWeights.reserve(newMesh.mPositions.size());
		for (int i = 0;i<newMesh.mPositions.size();i++)
		{
			for (int j =0;j<MAX_BONE_INFLUENCE;j++)
			{
				newMesh.mBoneIDs[i][j] = -1;
				newMesh.mWeights[i][j] = 0.0f;
			}
		}

		auto& boneInfoMap = mBoneInfoMap;
		int& boneCount = mBoneCount;

		for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneID = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				BoneInfo boneInfo;
				boneInfo.id = boneCount;
				boneInfo.offset = glm::mat4(1.0f);
				boneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
				boneInfoMap[boneName] = boneInfo;
				boneID = boneCount;
				boneCount++;

			}
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
				assert(vertexId <= newMesh.mPositions.size());
				for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
				{	//find a pos set value
					if (newMesh.mBoneIDs[vertexId][i] < 0)
					{
						newMesh.mWeights[vertexId][i] = weight;
						newMesh.mBoneIDs[vertexId][i] = boneID;
						break;
					}
				}
			}
		}
	}
}