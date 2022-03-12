#include "DirectionalLight.h"
namespace TEngine {
	DirectionalLight::DirectionalLight(float intensity, const glm::vec3& lightColor, const glm::vec3& lightDir) :
		BaseLight(intensity, lightColor), mLightDir(lightDir)
	{
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	void DirectionalLight::SetupUniforms(Shader* shader, int currentLightIndex)
	{
		shader->SetUniform(("dirLights[" + std::to_string(currentLightIndex) + "].direction").c_str(), mLightDir);
		shader->SetUniform(("dirLights[" + std::to_string(currentLightIndex) + "].intensity").c_str(), mIntensity);
		shader->SetUniform(("dirLights[" + std::to_string(currentLightIndex) + "].lightColor").c_str(), mLightColor);
	}
	void DirectionalLight::OnGui() {
		BaseLight::OnGui();
		ImGui::DragFloat3("Dir",&mLightDir[0], DRAG_SPEED,-1,1);
	}
}