#include "ShadowmapPass.h"
#include "../../../../macros.h"
#include "../../../Camera.h"
#include "glm/gtc/matrix_transform.hpp"
namespace TEngine {
	ShadowmapPass::ShadowmapPass(Scene* scene) :
		RenderPass(scene)
	{
		// create shader
		std::unordered_map<std::string, std::string> shadowmapShaderPaths;
		shadowmapShaderPaths.insert({ "vertex","res/shader/common/ShadowmapGeneration.vert" });
		shadowmapShaderPaths.insert({ "fragment","res/shader/common/ShadowmapGeneration.frag" });
		mShadowmapGenShader = new Shader(shadowmapShaderPaths);

		// create framebuffer
		mShadowmapFB = new FrameBuffer(SHADOWMAP_RESOLUTION_X, SHADOWMAP_RESOLUTION_Y);

		// create depth texture settings and add depth attachment to framebuffer
		TextureSettings depthStencilTextureSettings;
		depthStencilTextureSettings.TextureFormat = GL_DEPTH_COMPONENT32; // Only use depth, cause we don't need stencil in shadowmap pass.
		depthStencilTextureSettings.TextureWrapSMode = GL_CLAMP_TO_BORDER;
		depthStencilTextureSettings.TextureWrapTMode = GL_CLAMP_TO_BORDER;
		depthStencilTextureSettings.TextureMagnificationFilterMode = GL_NEAREST;
		depthStencilTextureSettings.TextureMinificationFilterMode = GL_NEAREST;
		depthStencilTextureSettings.HasBorder = true;	// use default (1,1,1) border color
		depthStencilTextureSettings.HasMips = false;
		mShadowmapFB->AddDepthStencilTexture(depthStencilTextureSettings, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);

		// This function can tell opengl set glDrawBuffer(GL_NONE) and glReadBuffer(GL_NONE) when we don't set color texture
		mShadowmapFB->CreateFrameBuffer();
	}
	ShadowmapPass::~ShadowmapPass()
	{
		delete mShadowmapFB;
		delete mShadowmapGenShader;
	}

	ShadowmapPassOutput ShadowmapPass::Render()
	{
		// bind shadowmap framebuffer
		mShadowmapFB->Bind();
		mShadowmapFB->Clear();
		glViewport(0, 0, mShadowmapFB->GetWidth(), mShadowmapFB->GetHeight());

		// setup
		ModelRenderer* modelRenderer = mScene->GetModelRenderer();
		Camera* camera = mScene->GetCamera();
		LightManager* lightManager = mScene->GetLightManager();
		glm::vec3 directionalLightDir = lightManager->GetDirectionalLightDirection(0);

		// view setup
		mShadowmapGenShader->Bind();
		glm::vec3 dirLightShadowmapLookAtPos = camera->GetPosition() + (glm::normalize(camera->GetFront()) * 15.0f);
		//note the direction of DirLight is inverted, thus here is  + 
		glm::vec3 dirLightShadowmapEyePos = dirLightShadowmapLookAtPos + (glm::normalize(directionalLightDir) * 35.0f);
		glm::mat4 directionalLightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, SHADOWMAP_NEAR_PLANE, SHADOWMAP_FAR_PLANE);
		glm::mat4 directionalLightView = glm::lookAt(dirLightShadowmapEyePos, dirLightShadowmapLookAtPos, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 directionalLightViewProjMatrix = directionalLightProjection * directionalLightView;
		mShadowmapGenShader->SetUniform("lightSpaceMatrix", directionalLightViewProjMatrix);

		// add models to renderer
		mScene->AddModelsToRender();

		// set render modes
		modelRenderer->SetupRenderState(); // We do not consider transparent or one face objects now, assume all opaque objects.

		// render
		modelRenderer->Render(mShadowmapGenShader, false);

		// renderpass output
		ShadowmapPassOutput passOutput;
		passOutput.lightSpaceMatrix = directionalLightViewProjMatrix;
		passOutput.shadowmapFramebuffer = mShadowmapFB;

		return passOutput;
	}
}