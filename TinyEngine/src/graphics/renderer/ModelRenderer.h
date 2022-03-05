#pragma once
#include <deque>

#include "../model/shapes/Quad.h"
#include "../model/shapes/Cube.h"
#include "../Camera.h"
#include "../model/Model.h"
namespace TEngine {
	// ModelRenderer render all models which are in render queue
	class ModelRenderer
	{
	public:
		ModelRenderer(Camera* camera);
		~ModelRenderer();

		void SubmitRenderModel(Model* model);
		void SetupRenderState();

		void Render(Shader* shader, bool isUseMaterial);

	public:
		// ModelRenderer can render all the models in renderqueue or just NDC_Plane or NDC_Cube by NDC_xxx.Draw()
		Quad NDC_Plane;
		Cube NDC_Cube;

	private:
		void SetupModelMatrix(Model* model, Shader* shader, bool isUseMaterial);

	private:
		Camera* mCamera; // may for transparent object sort

		std::deque<Model*> mRenderQueue;  // model memory is managed outside
	};
}


