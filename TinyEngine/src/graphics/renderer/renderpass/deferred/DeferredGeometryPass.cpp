#include "DeferredGeometryPass.h"
#include "../../../../platform/window/WindowManager.h"
namespace TEngine {
	DeferredGeometryPass::DeferredGeometryPass(Scene* scene) :
		RenderPass(scene)
	{
		// create shader
		std::unordered_map<std::string, std::string> geometryShaderPaths;
		geometryShaderPaths.insert({ "vertex","res/shader/deferred/GeometryPass.vert" });
		geometryShaderPaths.insert({ "fragment","res/shader/deferred/GeometryPass.frag" });
		mGeometryShader = new Shader(geometryShaderPaths);

		// create GBuffer
		mGBuffer = new GBuffer(WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());
	}

	DeferredGeometryPass::~DeferredGeometryPass()
	{
		delete mGBuffer;
		delete mGeometryShader;
	}

	GeometryPassOutput DeferredGeometryPass::Render()
	{
		// detect window size change and resize it when necessary
		if (DetectWindowSizeChange(mGBuffer->GetWidth(), mGBuffer->GetHeight()))
			mGBuffer->ResizeFrameBuffer(WindowManager::Instance()->GetWidth(), WindowManager::Instance()->GetHeight());

		// bind gbuffer framebuffer
		mGBuffer->Bind();
		mGBuffer->Clear();
		glViewport(0, 0, mGBuffer->GetWidth(), mGBuffer->GetHeight());

		// setup
		ModelRenderer* modelRenderer = mScene->GetModelRenderer();
		Camera* camera = mScene->GetCamera();
		LightManager* lightManager = mScene->GetLightManager();

		// bind shader
		mGeometryShader->Bind();

		// set view, projection matrix, note the model matrix is set in modelrenderer
		mGeometryShader->SetUniform("view", camera->GetViewMatrix());
		mGeometryShader->SetUniform("projection", camera->GetProjectionMatrix());
		


		// add models to renderer
		mScene->AddModelsToRender();

		// set renderer modes
		modelRenderer->SetupRenderState();

		// enble stencil test
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		// render
		modelRenderer->Render(mGeometryShader, true);

		// disable stencil test
		glDisable(GL_STENCIL_TEST);

		return { mGBuffer };
	}
}