#pragma once
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
namespace TEngine {
	class Model
	{
	public:
		Model(const std::string& path);
		Model(const Mesh& mesh);
		Model(const std::vector<Mesh>& meshes);
		~Model();

		void Init();
		void Draw(Shader* shader, bool isUseMaterial) const;

		// Getters and Setters
		inline const glm::vec3& GetCenter() const { return mCenter; }
		inline const glm::vec3& GetPosition() const { return mPosition; }
		inline const glm::vec3& GetScale() const { return mScale; }
		inline const float& GetRotation() const { return mRotation; }
		inline const glm::vec3& GetRotateAxis() const { return mRotateAxis; }
		inline std::vector<Mesh>* GetMeshes() { return &mMeshes; }
		inline Mesh* GetMesh(int index = 0) { return &mMeshes[index]; }

		inline void SetPosition(const glm::vec3& pos) { mPosition = pos; }
		inline void SetScale(const glm::vec3& scale) { mScale = scale; }
		inline void SetRotation(const float rotation) { mRotation = rotation; }
		inline void SetRotateAxis(const glm::vec3& axis) { mRotateAxis = axis; }


	private:
		void LoadModel(const std::string& path);

		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		Texture* LoadMaterialTexture(aiMaterial* mat, aiTextureType type, bool isSRGB);

	private:
		std::vector<Mesh> mMeshes;
		std::string mDirectory;

		// model parameters
		glm::vec3 mCenter = glm::vec3(0.0f, 0.0f, 0.0f);		// center is from the original model data
		glm::vec3 mPosition = glm::vec3(0.0f, 0.0f, 0.0f);	// position is what we want to set
		glm::vec3 mScale = glm::vec3(1.0f, 1.0f, 1.0f); ;
		float mRotation = 0.0f;	 glm::vec3 mRotateAxis = glm::vec3(0.0f, 0.0f, 1.0f);

	};
}


