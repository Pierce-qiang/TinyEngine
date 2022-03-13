#include "Animator.h"
#include <iostream>
namespace TEngine {
	Animator::Animator(Animation* animation):m_CurrentTime(0.0),m_DeltaTime(0.0),m_CurrentAnimation(animation)
	{
		m_FinalBoneMatrices.reserve(100);
		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void Animator::UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = glm::mat4(1.0f);

		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}
	/*std::ostream& operator<<(std::ostream& stream, glm::mat4 in) {
		for (size_t i = 0; i < 4; i++) {
			stream << "[ ";
			for (size_t j = 0; j < 4; j++) {
				stream << in[i][j] << " , ";
			}
			stream << " ]\n";
		}
		return stream;
	}*/
	void Animator::SetFinalBoneMatrices(Shader* shader) {
		for (int i = 0; i <m_FinalBoneMatrices.size(); ++i)
		{
			if (MAX_BONES == i) break;;
			shader->SetUniform( ("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), m_FinalBoneMatrices[i]);
			//std::cout << "transform: " << i << "\n" << m_FinalBoneMatrices[i];
			
		}
	}

}