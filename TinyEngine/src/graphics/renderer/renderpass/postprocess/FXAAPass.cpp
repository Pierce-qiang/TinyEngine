#include "FXAAPass.h"
#include "../../../../platform/window/WindowManager.h"
namespace TEngine {
	FXAAPass::FXAAPass(Scene* scene) :
		RenderPass(scene)
	{
		// create shader
		std::unordered_map<std::string, std::string> fxaaShaderPaths;
		fxaaShaderPaths.insert({ "vertex","res/shader/postprocess/FXAA.vert" });
		fxaaShaderPaths.insert({ "fragment","res/shader/postprocess/FXAA.frag" });
		mFXAAShader = new Shader(fxaaShaderPaths);

		// create FBO
		mFXAAFBO = new FrameBuffer(WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());

		// add attachment and check whether success by call CreateFrameBuffer
		TextureSettings colorTextureSettings;
		colorTextureSettings.TextureFormat = GL_RGBA32F;
		colorTextureSettings.TextureWrapSMode = GL_CLAMP_TO_EDGE;
		colorTextureSettings.TextureWrapTMode = GL_CLAMP_TO_EDGE;
		colorTextureSettings.TextureMagnificationFilterMode = GL_LINEAR;
		colorTextureSettings.TextureMinificationFilterMode = GL_LINEAR;
		colorTextureSettings.HasMips = false;
		mFXAAFBO->AddColorTexture(colorTextureSettings, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		mFXAAFBO->AddDepthStencilRBO(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
		mFXAAFBO->CreateFrameBuffer();
	}

	FXAAPass::~FXAAPass()
	{
		delete mFXAAFBO;
		delete mFXAAShader;
	}

	PostProcessOutput FXAAPass::Render(LightingPassOutput dlightInput)
	{
		// disable depth and multisample
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_MULTISAMPLE);

		// detect window size change and resize it when necessary
		if (DetectWindowSizeChange(mFXAAFBO->GetWidth(), mFXAAFBO->GetHeight()))
			mFXAAFBO->ResizeFrameBuffer(WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());

		// bind framebuffer
		mFXAAFBO->Bind();
		mFXAAFBO->Clear();
		glViewport(0, 0, mFXAAFBO->GetWidth(), mFXAAFBO->GetHeight());

		// setup
		ModelRenderer* modelRenderer = mScene->GetModelRenderer();
		Camera* camera = mScene->GetCamera();
		LightManager* lightManager = mScene->GetLightManager();

		// set input textures
		mFXAAShader->Bind();
		dlightInput.fbo->GetColourTexture()->bind(10);
		mFXAAShader->SetUniform("inputTexture", 10);
		mFXAAShader->SetUniform("texelSize", glm::vec2(1.0f / mFXAAFBO->GetWidth(), 1.0f / mFXAAFBO->GetHeight()));

		// draw
		modelRenderer->NDC_Plane.Draw();

		// disable stencil test and reset depth test 
		glEnable(GL_DEPTH_TEST);

		return { mFXAAFBO };
	}
}