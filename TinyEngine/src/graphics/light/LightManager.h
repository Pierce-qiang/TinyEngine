#pragma once

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
namespace TEngine {
	class LightManager
	{
	public:
		LightManager();
		~LightManager();


		void BindCurrentLights(Shader* shader);
		void AddDirectionalLight(const DirectionalLight& directionalLight);
		void AddPointLight(const PointLight& pointLight);
		void AddSpotLight(const SpotLight& spotLight);

		const glm::vec3& GetDirectionalLightDirection(unsigned int index);
		void OnGui();
	private:
		void InitBasicLights();

	private:
		std::vector<DirectionalLight> mDirectionalLights;
		std::vector<PointLight> mPointLights;
		std::vector<SpotLight> mSpotLights;
	};
}


