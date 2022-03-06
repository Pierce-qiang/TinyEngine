#pragma once
#include "../RenderPass.h"
namespace TEngine {
	class FXAAPass : public RenderPass
	{
	public:
		FXAAPass(Scene* scene);
		~FXAAPass();

		PostProcessOutput Render(LightingPassOutput dlightInput);

	private:
		Shader* mFXAAShader;
		FrameBuffer* mFXAAFBO;
	};
}


