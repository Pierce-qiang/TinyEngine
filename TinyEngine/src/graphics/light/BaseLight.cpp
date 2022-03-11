#include "BaseLight.h"
namespace TEngine {
	BaseLight::BaseLight(float intensity, const glm::vec3& lightColor) :
		mIntensity(intensity), mLightColor(lightColor)
	{

	}

	BaseLight::~BaseLight()
	{
	}
	void BaseLight::OnGui() {
		ImGui::DragFloat("Intensity", &mIntensity, DRAG_SPEED, 0.01,10);
		ImGui::ColorEdit3("BaseColor", &mLightColor[0]);
	}
}