#include "UIRenderer.h"


#include "GameSystem.h"

#include "./Application/Helper.h"

#include "./Renderer/Sprite.h"
#include "./Renderer/RenderTarget.h"

#include "./RenderPass/RenderPasses.h"
#include "./Utilities/Log.h"

//--------------------------------------------------------------------------------------------------------------------------------

UIRenderer::UIRenderer(D3D::DevicePtr& p_device):
	mUIConfig(0)
{
	mpSpriteFont = std::make_unique<Sprite>(p_device, L"./Data/Fonts/font0.png");
	mpSprite = std::make_unique<Sprite>(p_device);
	mCurrentSettings.text = "";
	mCurrentSettings.pos = Vector2(16, SCREEN_HEIGHT - 8);
	mCurrentSettings.size = Vector2(16, 16);
	mCurrentSettings.color = Vector4(1, 1, 1, 1);
}

//--------------------------------------------------------------------------------------------------------------------------------

void UIRenderer::UpdateFrame()
{
#ifdef USE_IMGUI


	// imgui new frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif

}

//--------------------------------------------------------------------------------------------------------------------------------

void UIRenderer::BeginRenderingNewWindow(const char* window_title, bool isOpen)
{

#ifdef USE_IMGUI


	// code of render
	//static bool f_open = true;
	//ImGui::Begin(window_title, &mbFileOpen);
	ImGuiWindowFlags flags = mUIConfig;
	ImGui::Begin(window_title, &mbFileOpen, flags);


#endif // USE_IMGUI
}

//--------------------------------------------------------------------------------------------------------------------------------

void UIRenderer::RenderUIQueue()
{
#ifdef USE_IMGUI


	if (mUIClientsTable.empty()) return;

	for (auto& client : mUIClientsTable)
	{
		ImGui::Text("%s", client.first.c_str());

		client.second->RenderUI();

		ImGui::Separator();
	}

#endif // USE_IMGUI

}

//--------------------------------------------------------------------------------------------------------------------------------

void UIRenderer::FinishRenderingWindow()
{
#ifdef USE_IMGUI

	ImGui::End();

	mUIConfig = 0;


#endif // USE_IMGUI
}

void UIRenderer::Execute()
{
	//imgui render
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//--------------------------------------------------------------------------------------------------------------------------------

void UIRenderer::SetText(std::string text)
{
	mCurrentSettings.text = text;


	mFontDataQueue.emplace_back(mCurrentSettings);

	SetNextSpriteFontSettings(mCurrentSettings.pos - Vector2(0, FONT_SPACE), mCurrentSettings.size, mCurrentSettings.color);
}

//--------------------------------------------------------------------------------------------------------------------------------

void UIRenderer::RenderSpriteFontQueue(D3D::DeviceContextPtr& p_imm_context)
{
	if (mFontDataQueue.empty()) return;

	ENGINE.GetRenderPass(RenderPassID::EForwardPass)->SetShader(p_imm_context, ShaderID::ESprite);


	for (auto& data : mFontDataQueue)
	{
		mpSpriteFont->TextOutput(p_imm_context, data.text, nullptr, data.pos, data.size, data.color);
	}

	ClearFontQueue();

}

void UIRenderer::RenderFullScreenQuad(D3D::DeviceContextPtr& p_imm_context)
{
	ENGINE.GetRenderPass(RenderPassID::EForwardPass)->SetShader(p_imm_context, ShaderID::ESprite);

}

//--------------------------------------------------------------------------------------------------------------------------------

void UIRenderer::ClearUIClients()
{
	mUIClientsTable.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------

void UIRenderer::ClearFontQueue()
{
	mFontDataQueue.clear();
	mCurrentSettings.text = "";
	mCurrentSettings.pos = Vector2(16, SCREEN_HEIGHT - 8);
	mCurrentSettings.size = Vector2(16, 16);
	mCurrentSettings.color = Vector4(1, 1, 1, 1);

}

//--------------------------------------------------------------------------------------------------------------------------------

void UIRenderer::SetNextWindowSettings(const Vector2 &pos, const Vector2 &size, const Vector2& pivot)
{
	ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y), ImGuiCond_Once, ImVec2(pivot.x, pivot.y));
	ImGui::SetNextWindowSize(ImVec2(size.x, size.y), ImGuiCond_Once);

}

void UIRenderer::SetNextUIConfig(bool b_menubar)
{
	mUIConfig = 0;
	if (b_menubar) mUIConfig |= ImGuiWindowFlags_MenuBar;
}

//--------------------------------------------------------------------------------------------------------------------------------

void UIRenderer::SetNextSpriteFontSettings(const Vector2& pos, const Vector2& size, const Vector4& color)
{
	mCurrentSettings.pos = pos;
	mCurrentSettings.size = size;
	mCurrentSettings.color = color;
}

//--------------------------------------------------------------------------------------------------------------------------------
