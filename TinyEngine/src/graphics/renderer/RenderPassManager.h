#pragma once
#include "renderpass/forward/ForwardLightingPass.h"
#include "renderpass/common/ShadowmapPass.h"
#include "renderpass/deferred/DeferredLightingPass.h"
#include "renderpass/deferred/DeferredGeometryPass.h"
#include "renderpass/postprocess/FXAAPass.h"
#include "renderpass/postprocess/PassThroughPass.h"
#include "renderpass/postprocess/SSRPass.h"
#include <unordered_map>
#include <string>
#include "../../platform/opengl/FrameBuffer.h"
namespace TEngine {
	class RenderPassManager
	{
	public:
		RenderPassManager(Scene* scene);
		~RenderPassManager();

		void Render();
		void SaverRenderFrame(const std::string& savePath);
		std::unordered_map<std::string, FrameBuffer*>& GetFrameBuffers() { return outPutframeBuffer; }
	private:
		Scene* mScene;  // mScene's memory is managed outside


		ShadowmapPass  mShadowmapPass;
		FowardLightingPass mFowardLightingPass;
		DeferredGeometryPass mDeferredGeometryPass;
		DeferredLightingPass mDeferredLightingPass;
		SSRPass mSSRPass;
		FXAAPass mFXAAPass;
		PassThroughPass mPS;
		std::unordered_map<std::string, FrameBuffer*> outPutframeBuffer;
	};
}


