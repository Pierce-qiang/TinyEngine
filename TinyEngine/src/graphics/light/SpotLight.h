#pragma once

#include "BaseLight.h"
#include "glm/glm.hpp"
namespace TEngine {
	class SpotLight : public BaseLight
	{
	public:
		SpotLight(float intensity, const glm::vec3& lightColor, float attenuationRadius,
			const glm::vec3& lightPos, const glm::vec3& lightDir, float innerCutOffAngle, float outerCutOffAngle);
		virtual ~SpotLight();

		virtual void SetupUniforms(Shader* shader, int currentLightIndex) override;
		void OnGui()override;
	private:
		float mAttenuationRadius;
		float mInnerCutOff, mOuterCutOff;
		glm::vec3 mLightPos, mLightDir;
		

	};
}

