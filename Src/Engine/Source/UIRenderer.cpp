#include "UIRenderer.h"

#include "./Renderer/Sprite.h"

UIRenderer::UIRenderer()
{
}

void UIRenderer::RenderImGui(float elapsed_time)
{
#ifdef USE_IMGUI

	// imgui new frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_Once);


	// code of render
	static bool f_open = true;
	ImGui::Begin("Debugging", &f_open);

	for (auto& client : m_UIClients)
	{
		ImGui::Text("%s", client.first.c_str());

		client.second->RenderUI();

		ImGui::Separator();
	}

	ImGui::End();


	ImGui::SetNextWindowPos(ImVec2(0, 600), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_Once);

	ImGui::Begin("Test2", &f_open);


	ImGui::End();


	//imgui render
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

#endif // USE_IMGUI

}
