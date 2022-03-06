#include "SSRPass.h"
#include "../../../../platform/window/WindowManager.h"
namespace TEngine {
	SSRPass::SSRPass(Scene* scene) :
		RenderPass(scene)
	{
		// create shader
		std::unordered_map<std::string, std::string> ssrShaderPaths;
		ssrShaderPaths.insert({ "vertex","res/shader/postprocess/SSRPass.vert" });
		ssrShaderPaths.insert({ "fragment","res/shader/postprocess/SSRPass.frag" });
		mSSRShader = new Shader(ssrShaderPaths);

		// create FBO
		mSSRFBO = new FrameBuffer(WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());


		// add attachment and check whether success by call CreateFrameBuffer
		TextureSettings colorTextureSettings;
		colorTextureSettings.TextureFormat = GL_RGBA32F;
		colorTextureSettings.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		colorTextureSettings.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		colorTextureSettings.TextureMagnificationFilterMode = GL_LINEAR;
		colorTextureSettings.TextureMinificationFilterMode = GL_LINEAR;
		colorTextureSettings.HasMips = false;
		mSSRFBO->AddColorTexture(colorTextureSettings, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		mSSRFBO->AddDepthStencilRBO(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
		mSSRFBO->CreateFrameBuffer();
	}

	SSRPass::~SSRPass()
	{
		delete mSSRFBO;
		delete mSSRShader;
	}

	SSRPassOutput SSRPass::Render(GeometryPassOutput gInput, LightingPassOutput dlightInput)
	{
		// disable depth and multisample
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_MULTISAMPLE);

		// detect window size change and resize it when necessary
		if (DetectWindowSizeChange(mSSRFBO->GetWidth(), mSSRFBO->GetHeight()))
			mSSRFBO->ResizeFrameBuffer(WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());

		// bind lighting framebuffer
		mSSRFBO->Bind();
		mSSRFBO->Clear();
		glViewport(0, 0, mSSRFBO->GetWidth(), mSSRFBO->GetHeight());

		// Move the depth + stencil of the GBuffer to the our framebuffer
		// NOTE: Framebuffers have to have identical depth + stencil formats for this to work
		// NOTE: after the blit we still bind the mSSRFBO as DRAW_FBO
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gInput.outputGBuffer->GetFramebuffer());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSRFBO->GetFramebuffer());
		glBlitFramebuffer(0, 0, gInput.outputGBuffer->GetWidth(), gInput.outputGBuffer->GetHeight(), 0, 0, mSSRFBO->GetWidth(), mSSRFBO->GetHeight(), GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

		// setup
		ModelRenderer* modelRenderer = mScene->GetModelRenderer();
		Camera* camera = mScene->GetCamera();
		LightManager* lightManager = mScene->GetLightManager();

		// set view position and matrix
		mSSRShader->Bind();
		mSSRShader->SetUniform("viewPos", camera->GetPosition());
		mSSRShader->SetUniform("viewMatrix", camera->GetViewMatrix());
		mSSRShader->SetUniform("invViewMatrix", glm::inverse(camera->GetViewMatrix()));
		mSSRShader->SetUniform("projectionMatrix", camera->GetProjectionMatrix());

		// set enabled
		mSSRShader->SetUniform("enabled", mEnabled);

		// set input textures
		gInput.outputGBuffer->GetRenderTarget(1)->bind(4);
		mSSRShader->SetUniform("normalTexture", 4);
		gInput.outputGBuffer->GetRenderTarget(2)->bind(5);
		mSSRShader->SetUniform("positionTexture", 5);
		gInput.outputGBuffer->GetRenderTarget(3)->bind(6);
		mSSRShader->SetUniform("mixtureTexture", 6);
		dlightInput.fbo->GetColourTexture()->bind(7);
		mSSRShader->SetUniform("colorTexture", 7);

		// enble stencil test
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);

		// draw
		modelRenderer->NDC_Plane.Draw();

		// disable stencil test and reset depth test 
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);


		return { mSSRFBO };
	}
}