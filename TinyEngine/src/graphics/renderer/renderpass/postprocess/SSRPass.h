#pragma once
#include "../RenderPass.h"
namespace TEngine {
	class SSRPass : public RenderPass
	{
	public:
		SSRPass(Scene* scene);
		~SSRPass();

		SSRPassOutput Render(GeometryPassOutput gInput, LightingPassOutput dlightInput);

	private:
		Shader* mSSRShader;
		FrameBuffer* mSSRFBO;

		bool mEnabled = true;
	};
}


