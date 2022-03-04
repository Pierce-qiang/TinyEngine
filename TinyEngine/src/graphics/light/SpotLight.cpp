#include "SpotLight.h"
namespace TEngine {
	SpotLight::SpotLight(float intensity, const glm::vec3& lightColor, float attenuationRadius, const glm::vec3& lightPos, const glm::vec3& lightDir, float innerCutOffAngle, float outerCutOffAngle) :
		BaseLight(intensity, lightColor), mAttenuationRadius(attenuationRadius), mLightDir(lightDir),
		mLightPos(lightPos), mInnerCutOff(innerCutOffAngle), mOuterCutOff(outerCutOffAngle)
	{
	}

	SpotLight::~SpotLight()
	{
	}

	void SpotLight::SetupUniforms(Shader* shader, int currentLightIndex)
	{
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].position").c_str(), mLightPos);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].direction").c_str(), mLightDir);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].intensity").c_str(), mIntensity);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].lightColor").c_str(), mLightColor);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].attenuationRadius").c_str(), mAttenuationRadius);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].innerCutOff").c_str(), mInnerCutOff);
		shader->SetUniform(("spotLights[" + std::to_string(currentLightIndex) + "].outerCutOff").c_str(), mOuterCutOff);
	}

}