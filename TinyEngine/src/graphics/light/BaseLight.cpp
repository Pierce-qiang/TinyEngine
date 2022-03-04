#include "BaseLight.h"
namespace TEngine {
	BaseLight::BaseLight(float intensity, const glm::vec3& lightColor) :
		mIntensity(intensity), mLightColor(lightColor)
	{

	}

	BaseLight::~BaseLight()
	{
	}
}