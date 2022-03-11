#include "PointLight.h"
namespace TEngine {
	PointLight::PointLight(float intensity, const glm::vec3& lightColor, float attenuationRadius, const glm::vec3& lightPos) :
		BaseLight(intensity, lightColor), mAttenuationRadius(attenuationRadius), mLightPos(lightPos)
	{
	}

	PointLight::~PointLight()
	{
	}

	void PointLight::SetupUniforms(Shader* shader, int currentLightIndex)
	{
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].position").c_str(), mLightPos);
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].intensity").c_str(), mIntensity);
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].lightColor").c_str(), mLightColor);
		shader->SetUniform(("pointLights[" + std::to_string(currentLightIndex) + "].attenuationRadius").c_str(), mAttenuationRadius);
	}

	void PointLight::OnGui() {
		BaseLight::OnGui();
		ImGui::DragFloat("AttenuationRadius", &mAttenuationRadius,DRAG_SPEED,1,10);
		ImGui::DragFloat3("WorldPos", &mLightPos[0],DRAG_SPEED);
	}

}