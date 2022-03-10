#include "GuiManager.h"
#include <unordered_map>
namespace TEngine {
	GuiManager::GuiManager(Scene* scene, RenderPassManager* renderPassManager) :mScene(scene), mRenderpassManager(renderPassManager) {
#pragma region imgui
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(WindowManager::Instance()->GetWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 430");
#pragma endregion
	}
	GuiManager::~GuiManager() {
	}
	void GuiManager::EndGui() {
		// cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	void GuiManager::Draw() {
		PrepareFrame();

		ImGui::ShowDemoWindow();
		//TODO: Write Gui logic------------------------
		auto outputBuffers = mRenderpassManager->GetFrameBuffers();
		for (auto& item : outputBuffers) {
			if ("Scene"==item.first)
			{
				item.second->GetColourTexture()->displayTexture(item.first);
			}
			else if("GeometryPass_Output"==item.first)
			{
				GBuffer* gOutput = dynamic_cast<GBuffer*>(item.second);
				assert(gOutput != nullptr);
				auto& textures = gOutput->GetColourTextures();
				

			}
		}



		EndFrame();
	}
	void GuiManager::PrepareFrame() {
		//begin imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport();
	}

	void GuiManager::EndFrame() {
#pragma region endFrame
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		glfwGetFramebufferSize(WindowManager::Instance()->GetWindow(), &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#pragma endregion
	}
}