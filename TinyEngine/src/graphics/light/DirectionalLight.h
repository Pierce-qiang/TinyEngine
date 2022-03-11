#pragma once

#include "BaseLight.h"
#include "../Shader.h"
#include "glm/glm.hpp"
namespace TEngine {
	class LightManager;

	class DirectionalLight : public BaseLight
	{
		friend LightManager; //need visit mLightDir in shadow map procedure
	public:
		DirectionalLight(float intensity, const glm::vec3& lightColor, const glm::vec3& lightDir);
		virtual ~DirectionalLight();

		virtual void SetupUniforms(Shader* shader, int currentLightIndex) override;
		void OnGui()override;
	private:
		glm::vec3 mLightDir;
	};
}


