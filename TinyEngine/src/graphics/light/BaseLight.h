#pragma once
#include "glm/glm.hpp"
#include "../Shader.h"

namespace TEngine {
	class BaseLight
	{
	public:
		BaseLight(float intensity, const glm::vec3& lightColor);
		virtual ~BaseLight();

		virtual void SetupUniforms(Shader* shader, int currentLightIndex) = 0;

	protected:
		float mIntensity;
		glm::vec3 mLightColor;
	};
}


