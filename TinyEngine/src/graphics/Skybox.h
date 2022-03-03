#pragma once
#include <vector>
#include <string>

#include "./texture/Cubemap.h"
#include "./Camera.h"
#include "./model/shapes/Cube.h"
#include "./Shader.h"
namespace TEngine {
	class Skybox
	{
	public:
		Skybox(const std::vector<std::string>& paths);
		~Skybox();

		void Render(Camera* camera);

	private:
		Cube mCube;
		Cubemap* mCubemap;
		Shader* mSkyboxShader;
	};
}


