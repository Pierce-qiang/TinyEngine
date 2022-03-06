#pragma once
#include "../../../Scene.h"
#include "../RenderPass.h"
namespace TEngine {
	class DeferredGeometryPass : public RenderPass
	{
	public:
		DeferredGeometryPass(Scene* scene);
		~DeferredGeometryPass();

		GeometryPassOutput Render();

	private:
		GBuffer* mGBuffer;
		Shader* mGeometryShader;
	};
}


