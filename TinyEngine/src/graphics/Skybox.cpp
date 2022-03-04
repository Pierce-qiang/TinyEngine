#include "Skybox.h"
#include "./texture/TextureLoader.h"
namespace TEngine {
	Skybox::Skybox(const std::vector<std::string>& paths)
	{
		// create shader
		std::unordered_map<std::string, std::string> geometryShaderPaths;
		geometryShaderPaths.insert({ "vertex","res/shader/common/Skybox.vert" });
		geometryShaderPaths.insert({ "fragment","res/shader/common/Skybox.frag" });
		mSkyboxShader = new Shader(geometryShaderPaths);

		// load cubemap
		CubemapSettings settings;
		settings.IsSRGB = true;
		mCubemap = TextureLoader::LoadCubemapTexture(paths, &settings);
	}

	Skybox::~Skybox()
	{
		delete mCubemap;
		delete mSkyboxShader;
	}

	void Skybox::Render(Camera* camera)
	{
		// bind shader and set uniforms
		mSkyboxShader->Bind();
		glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
		mSkyboxShader->SetUniform("view", view);
		mSkyboxShader->SetUniform("projection", camera->GetProjectionMatrix());

		mCubemap->Bind();
		mSkyboxShader->SetUniform("skybox", 0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);//<= make sure skybox depth which is 1.0 can be saw
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT); // because we are "in" the cube box

		mCube.Draw();

		glCullFace(GL_BACK);
		glDepthFunc(GL_LESS);
		mCubemap->UnBind();
	}
}