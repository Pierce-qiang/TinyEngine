#pragma once
#include "../RenderPass.h"
namespace TEngine {
	class PassThroughPass : public RenderPass
	{
	public:
		PassThroughPass(Scene* scene);
		~PassThroughPass();

		void Render(GeometryPassOutput gInput, FrameBuffer* colorFBO, SSRPassOutput ssrInput = { nullptr }, ShadowmapPassOutput smInput = { glm::mat4(1.0f), nullptr });

	private:
		Shader* mPassThroughShader;
	};
}


