#include "RenderPass.h"
#include "../../../platform/window/WindowManager.h"
namespace TEngine {
	RenderPass::RenderPass(Scene* scene)
		:mScene(scene)
	{
		// create debug shader
		std::unordered_map<std::string, std::string> debugShaderPaths;
		debugShaderPaths.insert({ "vertex","res/shader/common/DebugDisplay.vert" });
		debugShaderPaths.insert({ "fragment","res/shader/common/DebugDisplay.frag" });
		mDebugShader = new Shader(debugShaderPaths);
	}

	RenderPass::~RenderPass()
	{
		delete mDebugShader;
	}

	// displayChannel 1:red / 2:green / 3:blue / 4:rgb
	void RenderPass::DisplayTexture(int startx, int starty, int width, int height,
		Texture* texture, int displayChannel /*= 4*/, int unit /* = 9  */)
	{
		// bind default framebuffer
		WindowManager::Bind();

		// bind debug shader
		mDebugShader->Bind();

		// set the texture display position
		glViewport(startx, starty, width, height);

		// bind texture
		texture->bind(unit);
		mDebugShader->SetUniform("displayTexture", unit);
		mDebugShader->SetUniform("displayChannel", displayChannel);

		// draw ndc plane
		mScene->GetModelRenderer()->NDC_Plane.Draw();
	}

	bool  RenderPass::DetectWindowSizeChange(int framebufferWidth, int framebufferHeight)
	{
		int windowWidth = WindowManager::Instance()->GetWidth();
		int windowHeight = WindowManager::Instance()->GetHeight();

		return (windowWidth != framebufferWidth || windowHeight != framebufferHeight);
	}
}