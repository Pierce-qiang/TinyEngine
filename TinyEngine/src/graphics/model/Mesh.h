#pragma once
#include <vector>

#include "./Material.h"
#include "./glm/glm.hpp"
namespace TEngine {
	class Model;
	class Mesh
	{
	friend Model;

	public:
		Mesh();
		Mesh(std::vector<glm::vec3>& positions, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<unsigned int>& indices);
		Mesh(std::vector<glm::vec3>& positions, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents, std::vector<unsigned int>& indices);
		~Mesh();
		void SetBoneInfluence(std::vector<glm::ivec4>& BoneID, std::vector<glm::vec4>& BoneWeight);

		void Draw() const;
		void OnGui();

		inline bool HasTangents() const { return mTangents.size() > 0; }

		inline Material* GetMaterial() { return &mMaterial; }

	protected:
		void SetupMesh(bool interleaved = true);

	protected:
		std::vector<glm::vec3> mPositions;
		std::vector<glm::vec2> mUVs;
		std::vector<glm::vec3> mNormals;
		std::vector<glm::vec3> mTangents;
		std::vector<glm::vec3> mBitangents;
		//bone indexes which will influence this vertex
		std::vector<glm::ivec4> mBoneIDs;
		//weights from each bone
		std::vector<glm::vec4> mWeights;

		std::vector<unsigned int> mIndices;

		unsigned int mVAO, mVBO, mEBO;
		Material mMaterial;  // initialize with default construct i.e. all material texture is nullptr
	};
}


