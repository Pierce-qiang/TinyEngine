#pragma once
#include "Scene.h"
#include "imgui/imgui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../platform/window/WindowManager.h"
#include "renderer/RenderPassManager.h"
namespace TEngine {
	class GuiManager
	{
	public:
		GuiManager(Scene* scene, RenderPassManager* renderPassManager);
		~GuiManager();
		void Draw();
		void EndGui();
	private:
		Scene* mScene;
		RenderPassManager* mRenderpassManager;

		void PrepareFrame();
		void EndFrame();

	};
	
}


