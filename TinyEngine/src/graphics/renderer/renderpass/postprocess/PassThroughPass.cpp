#include "PassThroughPass.h"
#include "../../../../platform/window/WindowManager.h"
namespace TEngine {
	PassThroughPass::PassThroughPass(Scene* scene) :
		RenderPass(scene)
	{
		// create shader
		std::unordered_map<std::string, std::string> passThroughShaderPaths;
		passThroughShaderPaths.insert({ "vertex","res/shader/postprocess/PassThrough.vert" });
		passThroughShaderPaths.insert({ "fragment","res/shader/postprocess/PassThrough.frag" });
		mPassThroughShader = new Shader(passThroughShaderPaths);
	}

	PassThroughPass::~PassThroughPass()
	{
		delete mPassThroughShader;
	}

	void PassThroughPass::Render(GeometryPassOutput gInput, FrameBuffer* colorFBO, SSRPassOutput ssrInput /*= {nullptr}*/)
	{
		glDisable(GL_DEPTH_TEST);

		WindowManager::Instance()->Bind();
		WindowManager::Instance()->Clear();
		glViewport(0, 0, WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());

		// setup
		ModelRenderer* modelRenderer = mScene->GetModelRenderer();
		Camera* camera = mScene->GetCamera();
		LightManager* lightManager = mScene->GetLightManager();

		// bind shader
		mPassThroughShader->Bind();

		// set ssr and deferred lighting
		if (ssrInput.ssrFBO != nullptr)
		{
			ssrInput.ssrFBO->GetColourTexture()->bind(6);
			mPassThroughShader->SetUniform("ssrTexture", 6);
		}
		colorFBO->GetColourTexture()->bind(7);
		mPassThroughShader->SetUniform("colorTexture", 7);


		modelRenderer->NDC_Plane.Draw();

		
		DisplayTexture(0, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(0), 4, 10);
		DisplayTexture(150, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(1), 4, 11);
		DisplayTexture(300, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(2), 4, 12);
		DisplayTexture(450, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(3), 1, 13);
		DisplayTexture(600, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(3), 2, 14);
		DisplayTexture(750, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(3), 3, 15);
		//DisplayTexture(900, 0, 150, 150, gInput.outputGBuffer->GetDepthStencilTexture(), 1, 16);
		//DisplayTexture(1050, 0, 150,150 , ssrInput.ssrFBO->GetColourTexture(), 4, 8);
		glEnable(GL_DEPTH_TEST);
	}
}