#include "RenderPassManager.h"
#include "../../platform/window/WindowManager.h"
#include "imgui/imgui.h"
namespace TEngine {
	RenderPassManager::RenderPassManager(Scene* scene) :
		mScene(scene), mFowardLightingPass(scene), mShadowmapPass(scene),
		mDeferredGeometryPass(scene), mDeferredLightingPass(scene), mSSRPass(scene),
		mFXAAPass(scene), mPS(scene)
	{
		// always enable this to let sample cube map seamlessly
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	RenderPassManager::~RenderPassManager()
	{

	}

	void RenderPassManager::Render()
	{
		// it's a simple test now
		//ShadowmapPassOutput smOutput = mShadowmapPass.Render();
		//mFowardLightingPass.Render(smOutput, true);

		ShadowmapPassOutput smOutput = mShadowmapPass.Render();
		GeometryPassOutput gOutput = mDeferredGeometryPass.Render();
		LightingPassOutput lightOutput = mDeferredLightingPass.Render(gOutput, smOutput);
		SSRPassOutput ssrOutput = mSSRPass.Render(gOutput, lightOutput);
		PostProcessOutput fxaaOutput = mFXAAPass.Render(lightOutput);

		//mPS.Render(gOutput, lightOutput.deferredLightingFBO, ssrOutput);
		mPS.Render(gOutput, fxaaOutput.fbo, ssrOutput, smOutput);
		outPutframeBuffer.insert({ "ShadowMap_Output", smOutput.shadowmapFramebuffer });
		outPutframeBuffer.insert({ "GeometryPass_Output", gOutput.outputGBuffer });
		outPutframeBuffer.insert({ "LightPass_Output", lightOutput.fbo });
		outPutframeBuffer.insert({ "SSRPass_Output", ssrOutput.ssrFBO });
		outPutframeBuffer.insert({ "FXAA_Output", fxaaOutput.fbo });
		outPutframeBuffer.insert({ "Scene", mPS.GetFrameBuffer() });

	}
}