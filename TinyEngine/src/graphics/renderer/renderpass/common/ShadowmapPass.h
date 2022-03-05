#pragma once
#include "../RenderPass.h"
namespace TEngine {
	class ShadowmapPass : public RenderPass
	{
	public:
		ShadowmapPass(Scene* scene);
		virtual ~ShadowmapPass() override;

		ShadowmapPassOutput Render();

	private:
		FrameBuffer* mShadowmapFB;
		Shader* mShadowmapGenShader;
	};
}


