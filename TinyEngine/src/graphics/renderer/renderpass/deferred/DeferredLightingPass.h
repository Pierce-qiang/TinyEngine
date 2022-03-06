#pragma once
#include "../../../Scene.h"
#include "../RenderPass.h"
namespace TEngine {
	class DeferredLightingPass : public RenderPass
	{
	public:
		DeferredLightingPass(Scene* scene);
		~DeferredLightingPass();

		LightingPassOutput Render(GeometryPassOutput gInput, ShadowmapPassOutput smInput);

	private:
		Shader* mLightingShader;
		FrameBuffer* mLightingFBO;
	};
}


