#pragma once
#include "BaseLight.h"
#include "glm/glm.hpp"
namespace TEngine {
	class PointLight : public BaseLight
	{
	public:
		PointLight(float intensity, const glm::vec3& lightColor, float attenuationRadius, const glm::vec3& lightPos);
		virtual ~PointLight();

		virtual void SetupUniforms(Shader* shader, int currentLightIndex) override;
		void OnGui()override;
	private:
		float mAttenuationRadius;
		glm::vec3 mLightPos;
	};
}


