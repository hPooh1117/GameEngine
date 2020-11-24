#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <array>

#include "UIClient.h"

#include "./Renderer/D3D_Helper.h"

#include "./Utilities/Vector.h"

class Sprite;

struct SpriteFontData
{
	Vector2 pos;
	Vector2 size;
	Vector4 color;
	std::string text;
};

class UIRenderer
{
private:
	std::map < std::string, std::shared_ptr<UIClient>> mUIClientsTable;
	std::unique_ptr<Sprite>                            mpSpriteFont;
	std::unique_ptr<Sprite>							   mpSprite;
	std::vector<SpriteFontData>                        mFontDataQueue;
	
	bool                                               mbFileOpen = false;
	SpriteFontData                                     mCurrentSettings;

public:
	UIRenderer() = default;
	UIRenderer(D3D::DevicePtr& p_device);
	~UIRenderer() = default;

	void UpdateFrame();
	void BeginRenderingNewWindow(const char* window_title, bool isOpen = true);
	void RenderUIQueue();
	void FinishRenderingWindow();
	void Execute();

	void RenderSpriteFontQueue(D3D::DeviceContextPtr& p_imm_context);
	void RenderFullScreenQuad(D3D::DeviceContextPtr& p_imm_context);

	void ClearUIClients();
	void ClearFontQueue();

	void SetInQueue(std::string ui_name, const std::shared_ptr<UIClient>& p_client)
	{
		mUIClientsTable.emplace(ui_name, p_client);
	}
	void SetText(std::string text);

	void SetNextWindowSettings(const Vector2& pos, const Vector2& size, const Vector2& pivot = Vector2(0,0));
	void SetNextSpriteFontSettings(const Vector2& pos, const Vector2& size, const Vector4& color);

	UIRenderer(const UIRenderer&) = delete;
	UIRenderer& operator=(const UIRenderer&) = delete;
};

