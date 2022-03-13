#pragma once
#include "Animation.h"
#include "../Shader.h"
namespace TEngine {
	class Animator
	{
	public:
		Animator(Animation* animation);

		void UpdateAnimation(float dt);

		void PlayAnimation(Animation* pAnimation)
		{
			m_CurrentAnimation = pAnimation;
			m_CurrentTime = 0.0f;
			m_DeltaTime = 0.0f;
		}

		void SetFinalBoneMatrices(Shader* shader);

		std::vector<glm::mat4> GetFinalBoneMatrices()
		{return m_FinalBoneMatrices; }

	private:
		void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

		std::vector<glm::mat4> m_FinalBoneMatrices;
		Animation* m_CurrentAnimation;
		float m_CurrentTime;
		float m_DeltaTime;

	};
}


