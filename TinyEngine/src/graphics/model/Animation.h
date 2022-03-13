#pragma once
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Bone.h"
#include "assimp/postprocess.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "../../macros.h"
namespace TEngine {
	struct BoneInfo
	{
		int id;
		//change model space to this bone space
		glm::mat4 offset;
	};

	struct AssimpNodeData
	{
		std::string name;
		int childrenCount;
		std::vector<AssimpNodeData> children;
	};

	class Animation
	{
	public:
		Animation() = default;

		Animation(const std::string& animationPath, std::unordered_map<std::string, BoneInfo>& BoneInfoMap, int& BoneCount);
		~Animation(){}

		Bone* FindBone(const std::string& name);


		inline float GetTicksPerSecond() { return m_TicksPerSecond; }
		inline float GetDuration() { return m_Duration; }
		inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
		inline const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap()
		{return m_BoneInfoMap; }

	private:
		void ReadMissingBones(const aiAnimation* animation, std::unordered_map<std::string, BoneInfo>& BoneInfoMap, int& BoneCount);
		void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
		
		float m_Duration;
		float m_TicksPerSecond;
		std::vector<Bone> m_Bones;
		AssimpNodeData m_RootNode;//heirarchy
		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
	};
}


