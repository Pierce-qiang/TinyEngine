#include "ModelRenderer.h"
#include <iostream>
#include "./glm/gtc/matrix_transform.hpp"
namespace TEngine {
	//convinent ostream to debug
	std::ostream& operator<<(std::ostream& stream, const glm::mat4& matrix) {

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				std::cout << matrix[i][j] << " ";
			}
			std::cout << std::endl;
		}

		return stream;
	}

	ModelRenderer::ModelRenderer(Camera* camera)
		:mCamera(camera)
	{
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
	}

	ModelRenderer::~ModelRenderer()
	{
	}

	void ModelRenderer::SubmitRenderModel(Model* model)
	{
		mRenderQueue.push_back(model);
	}

	void ModelRenderer::SetupRenderState()
	{
		// This is for opaque objects;
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void ModelRenderer::Render(Shader* shader, bool isUseMaterial)
	{
		// render all objects
		while (!mRenderQueue.empty())
		{
			//std::cout << "here\n";
			Model* current = mRenderQueue.front();

			// We setup model matrix here, setup materials in model's draw and setup light parameters in renderpass.
			SetupModelMatrix(current, shader, isUseMaterial);
			current->Draw(shader, isUseMaterial);

			mRenderQueue.pop_front();
		}
	}

	void ModelRenderer::SetupModelMatrix(Model* model, Shader* shader, bool isUseMaterial)
	{
		glm::mat4 modelMatrix(1.0f);
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), model->GetPosition());

		glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(model->GetRotation()), model->GetRotateAxis());
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), model->GetScale());
		glm::mat4 centerTranslate = glm::translate(glm::mat4(1.0f), (-model->GetCenter()));

		//std::cout << "================================\n";
		//std::cout << glm::to_string(centerTranslate) << "\n";
		//std::cout << (centerTranslate) << "\n";
		//std::cout << "================================\n";

		// TODO: calculate the model's center and move it to origin first
		modelMatrix = translate * rotate * scale * centerTranslate;
		shader->SetUniform("model", modelMatrix);

		if (isUseMaterial)
		{
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
			shader->SetUniform("normalMatrix", normalMatrix);
		}
	}
}