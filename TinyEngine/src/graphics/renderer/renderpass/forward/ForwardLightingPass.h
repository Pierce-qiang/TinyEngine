#pragma once
#include "../RenderPass.h"
namespace TEngine {
	class FowardLightingPass : public RenderPass
	{
	public:
		FowardLightingPass(Scene* scene);
		~FowardLightingPass();

		void Render(const ShadowmapPassOutput& smOutput, bool isUsePBR = true);

	private:
		Shader* mBlinnShader;
		Shader* mPBRShader;
	};
}


