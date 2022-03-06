#include "DeferredLightingPass.h"
#include "../../../../platform/window/WindowManager.h"
namespace TEngine {
	DeferredLightingPass::DeferredLightingPass(Scene* scene) :
		RenderPass(scene)
	{
		// create shader
		std::unordered_map<std::string, std::string> lightingShaderPaths;
		lightingShaderPaths.insert({ "vertex","res/shader/deferred/LightingPass.vert" });
		lightingShaderPaths.insert({ "fragment","res/shader/deferred/LightingPass.frag" });
		mLightingShader = new Shader(lightingShaderPaths);

		// create FBO
		mLightingFBO = new FrameBuffer(WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());

		// add attachment and check whether success by call CreateFrameBuffer
		TextureSettings colorTextureSettings;
		colorTextureSettings.TextureFormat = GL_RGBA32F;
		colorTextureSettings.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		colorTextureSettings.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		colorTextureSettings.TextureMagnificationFilterMode = GL_LINEAR;
		colorTextureSettings.TextureMinificationFilterMode = GL_LINEAR;
		colorTextureSettings.HasMips = false;
		mLightingFBO->AddColorTexture(colorTextureSettings, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);

		TextureSettings depthStencilTextureSettings;
		depthStencilTextureSettings.TextureFormat = GL_DEPTH24_STENCIL8;
		depthStencilTextureSettings.TextureWrapSMode = GL_CLAMP_TO_BORDER;
		depthStencilTextureSettings.TextureWrapTMode = GL_CLAMP_TO_BORDER;
		depthStencilTextureSettings.TextureMagnificationFilterMode = GL_NEAREST;
		depthStencilTextureSettings.TextureMinificationFilterMode = GL_NEAREST;
		depthStencilTextureSettings.HasBorder = true;	// use default (1,1,1) border color
		depthStencilTextureSettings.HasMips = false;
		mLightingFBO->AddDepthStencilTexture(depthStencilTextureSettings, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL_ATTACHMENT);

		//mLightingFBO->AddDepthStencilRBO(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
		mLightingFBO->CreateFrameBuffer();

	}

	DeferredLightingPass::~DeferredLightingPass()
	{
		delete mLightingFBO;
		delete mLightingShader;
	}

	LightingPassOutput DeferredLightingPass::Render(GeometryPassOutput gInput, ShadowmapPassOutput smInput)
	{
		// disable depth and multisample
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_MULTISAMPLE);

		// detect window size change and resize it when necessary
		if (DetectWindowSizeChange(mLightingFBO->GetWidth(), mLightingFBO->GetHeight()))
			mLightingFBO->ResizeFrameBuffer(WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());

		// bind lighting framebuffer
		mLightingFBO->Bind();
		mLightingFBO->Clear();
		glViewport(0, 0, mLightingFBO->GetWidth(), mLightingFBO->GetHeight());

		// Move the depth + stencil of the GBuffer to the our framebuffer
		// NOTE: Framebuffers have to have identical depth + stencil formats for this to work
		// NOTE: after the blit we still bind the mLightingFBO as DRAW_FBO
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gInput.outputGBuffer->GetFramebuffer());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mLightingFBO->GetFramebuffer());
		glBlitFramebuffer(0, 0, gInput.outputGBuffer->GetWidth(), gInput.outputGBuffer->GetHeight(), 0, 0, mLightingFBO->GetWidth(), mLightingFBO->GetHeight(), GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

		// setup
		ModelRenderer* modelRenderer = mScene->GetModelRenderer();
		Camera* camera = mScene->GetCamera();
		LightManager* lightManager = mScene->GetLightManager();

		// set view position
		mLightingShader->Bind();
		mLightingShader->SetUniform("viewPos", camera->GetPosition());

		// set light
		lightManager->BindCurrentLights(mLightingShader);

		// set shadowmap 
		smInput.shadowmapFramebuffer->GetDepthStencilTexture()->bind(0);
		mLightingShader->SetUniform("shadowMap", 0);
		mLightingShader->SetUniform("lightSpaceMatrix", smInput.lightSpaceMatrix);

		// set gbuffet
		gInput.outputGBuffer->GetRenderTarget(0)->bind(4);
		mLightingShader->SetUniform("albedoTexture", 4);
		gInput.outputGBuffer->GetRenderTarget(1)->bind(5);
		mLightingShader->SetUniform("normalTexture", 5);
		gInput.outputGBuffer->GetRenderTarget(2)->bind(6);
		mLightingShader->SetUniform("positionTexture", 6);
		gInput.outputGBuffer->GetRenderTarget(3)->bind(7);
		mLightingShader->SetUniform("mixtureTexture", 7);


		// enble stencil test for discarding unnecessary fragment shader
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);

		// draw objects
		modelRenderer->NDC_Plane.Draw();

		// debug display
		//DisplayTexture(0, 0, WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight(), mLightingFBO->GetColourTexture(), 4, 8);
		//DisplayTexture(0, 0,   150, 150, gInput.outputGBuffer->GetRenderTarget(0), 4, 10);
		//DisplayTexture(150, 0,  150, 150,gInput.outputGBuffer->GetRenderTarget(1), 4, 11);
		//DisplayTexture(300, 0,  150, 150,gInput.outputGBuffer->GetRenderTarget(2), 4, 12);
		//DisplayTexture(450, 0,  150, 150,gInput.outputGBuffer->GetRenderTarget(3), 1, 13);
		//DisplayTexture(600, 0,  150, 150,gInput.outputGBuffer->GetRenderTarget(3), 2, 14);
		//DisplayTexture(750, 0, 150, 150, gInput.outputGBuffer->GetRenderTarget(3), 3, 15);
		//DisplayTexture(900, 0, 150, 150, gInput.outputGBuffer->GetDepthStencilTexture(), 5, 16);

		// disable stencil test and reset depth test 
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);


		// draw skybox
		// 好像直接在这里draw天空盒就行了，因为depth buffer从 几何阶段拷贝过来了
		Skybox* skybox = mScene->GetSkybox();
		skybox->Render(camera);


		return { mLightingFBO };
	}
}