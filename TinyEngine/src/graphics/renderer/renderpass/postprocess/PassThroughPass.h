#pragma once
#include "../RenderPass.h"
namespace TEngine {
	class PassThroughPass : public RenderPass
	{
	public:
		PassThroughPass(Scene* scene);
		~PassThroughPass();

		void Render(GeometryPassOutput gInput, FrameBuffer* colorFBO, SSRPassOutput ssrInput = { nullptr });

	private:
		Shader* mPassThroughShader;
	};
}


