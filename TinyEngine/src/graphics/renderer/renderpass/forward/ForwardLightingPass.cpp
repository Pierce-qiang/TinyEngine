#include "ForwardLightingPass.h"
#include "../../../../platform/window/WindowManager.h"
namespace TEngine {
	FowardLightingPass::FowardLightingPass(Scene* scene) :
		RenderPass(scene)
	{
		// create blinn shader
		std::unordered_map<std::string, std::string> blinnShaderPaths;
		blinnShaderPaths.insert({ "vertex","res/shader/forward/BlinnPhong.vert" });
		blinnShaderPaths.insert({ "fragment","res/shader/forward/BlinnPhong.frag" });
		mBlinnShader = new Shader(blinnShaderPaths);

		// create pbr shader
		std::unordered_map<std::string, std::string> pbrShaderPaths;
		pbrShaderPaths.insert({ "vertex","res/shader/forward/PBRLighting.vert" });
		pbrShaderPaths.insert({ "fragment","res/shader/forward/PBRLighting.frag" });
		mPBRShader = new Shader(pbrShaderPaths);

	}

	FowardLightingPass::~FowardLightingPass()
	{
		delete mBlinnShader;
		delete mPBRShader;
	}

	void FowardLightingPass::Render(const ShadowmapPassOutput& smOutput, bool isUsePBR /* = true */)
	{
		// bind default framebuffer
		WindowManager::Bind();
		WindowManager::Clear();
		glViewport(0, 0, WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());

		// enable msaa
		glEnable(GL_MULTISAMPLE);

		// setup
		ModelRenderer* modelRenderer = mScene->GetModelRenderer();
		LightManager* lightManager = mScene->GetLightManager();
		Camera* camera = mScene->GetCamera();

		// get shadowmap pass output and bind shadowmap to texture unit 0 
		glm::mat4 lightSpaceMatrix = smOutput.lightSpaceMatrix;
		smOutput.shadowmapFramebuffer->GetDepthStencilTexture()->bind(0);

		if (isUsePBR)	// pbr shader
		{
			mPBRShader->Bind();
			mPBRShader->SetUniform("shadowMap", 0);
			mPBRShader->SetUniform("lightSpaceMatrix", lightSpaceMatrix);

			// set light
			lightManager->BindCurrentLights(mPBRShader);

			// set view, projection matrix, note the model matrix is set in modelrenderer
			mPBRShader->SetUniform("view", camera->GetViewMatrix());
			mPBRShader->SetUniform("viewPos", camera->GetPosition());
			mPBRShader->SetUniform("projection", camera->GetProjectionMatrix());

			// add models to renderer
			mScene->AddModelsToRender();

			// set renderer modes
			modelRenderer->SetupRenderState();

			// render
			modelRenderer->Render(mPBRShader, true);
		}
		else			// simple blinn shader
		{
			mBlinnShader->Bind();
			mBlinnShader->SetUniform("shadowMap", 0);
			mBlinnShader->SetUniform("lightSpaceMatrix", lightSpaceMatrix);

			// set view, projection matrix, note the model matrix is set in modelrenderer
			mBlinnShader->SetUniform("view", camera->GetViewMatrix());
			mBlinnShader->SetUniform("viewPos", camera->GetPosition());
			mBlinnShader->SetUniform("projection", camera->GetProjectionMatrix());

			// set a directional light
			mBlinnShader->SetUniform("directionalLightDir", glm::vec3(1.0f, 1.0f, 1.0f));

			// add models to renderer
			mScene->AddModelsToRender();

			// set renderer modes
			modelRenderer->SetupRenderState();

			// render
			modelRenderer->Render(mBlinnShader, true);
		}

		// just for debug
		//DisplayTexture(0, 0, 150, 150, smOutput.shadowmapFramebuffer->GetDepthStencilTexture(), 1, 10);
	}
}