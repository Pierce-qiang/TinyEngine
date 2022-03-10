#include "PassThroughPass.h"
#include "../../../../platform/window/WindowManager.h"
#include "imgui/imgui.h"
namespace TEngine {
	PassThroughPass::PassThroughPass(Scene* scene) :
		RenderPass(scene)
	{
		// create shader
		std::unordered_map<std::string, std::string> passThroughShaderPaths;
		passThroughShaderPaths.insert({ "vertex","res/shader/postprocess/PassThrough.vert" });
		passThroughShaderPaths.insert({ "fragment","res/shader/postprocess/PassThrough.frag" });
		mPassThroughShader = new Shader(passThroughShaderPaths);

		mfbo = new FrameBuffer(WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());

		// add attachment and check whether success by call CreateFrameBuffer
		TextureSettings colorTextureSettings;
		colorTextureSettings.TextureFormat = GL_RGBA32F;
		colorTextureSettings.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		colorTextureSettings.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		colorTextureSettings.TextureMagnificationFilterMode = GL_LINEAR;
		colorTextureSettings.TextureMinificationFilterMode = GL_LINEAR;
		colorTextureSettings.HasMips = false;
		mfbo->AddColorTexture(colorTextureSettings, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		//mfbo->AddDepthStencilRBO(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
		mfbo->CreateFrameBuffer();
	}

	PassThroughPass::~PassThroughPass()
	{
		delete mPassThroughShader;
	}

	void PassThroughPass::Render(GeometryPassOutput gInput, FrameBuffer* colorFBO, SSRPassOutput ssrInput /*= {nullptr}*/, ShadowmapPassOutput smInput /*= {glm::mat4(1.0f), nullptr}*/)
	{
		// disable depth and multisample
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_MULTISAMPLE);

		// detect window size change and resize it when necessary
		if (DetectWindowSizeChange(mfbo->GetWidth(), mfbo->GetHeight()))
			mfbo->ResizeFrameBuffer(WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());

		// bind lighting framebuffer
		mfbo->Bind();
		mfbo->Clear();
		glViewport(0, 0, mfbo->GetWidth(), mfbo->GetHeight());

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
		mfbo->UnBind();
		//early debug method;
		//DisplayTexture(0, 0, 150, 150, smInput.shadowmapFramebuffer->GetColourTexture(), 1, 0);
		////DisplayTexture(0, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(0), 4, 10);
		//DisplayTexture(150, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(1), 4, 11);
		//DisplayTexture(300, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(2), 4, 12);
		//DisplayTexture(450, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(3), 1, 13);
		//DisplayTexture(600, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(3), 2, 14);
		//DisplayTexture(750, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(3), 3, 15);
		//DisplayTexture(900, 0, 150, 150, gInput.outputGBuffer->GetDepthStencilTexture(), 1, 16);
		//DisplayTexture(1050, 0, 150,150 , ssrInput.ssrFBO->GetColourTexture(), 4, 8);
		glEnable(GL_DEPTH_TEST);
	}
}