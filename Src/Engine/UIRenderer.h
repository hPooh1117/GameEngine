#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <array>

#include "UIClient.h"

#include "./Renderer/D3D_Helper.h"
#include "./Renderer/GraphicsDevice.h"
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
public:
	static constexpr UINT FONT_SPACE = 16;

private:
	std::map < std::string, std::shared_ptr<UIClient>> mUIClientsTable;
	std::unique_ptr<Sprite>                            mpSpriteFont;
	std::unique_ptr<Sprite>							   mpSprite;
	std::vector<SpriteFontData>                        mFontDataQueue;
	
	bool                                               mbFileOpen = false;
	bool                                               mbIsUIEnable = true;
	SpriteFontData                                     mCurrentSettings;
	int                                                mUIConfig;

public:
	UIRenderer() = default;
	UIRenderer(Graphics::GraphicsDevice* device);
	~UIRenderer() = default;

	void UpdateFrame();
	void BeginRenderingNewWindow(const char* window_title, bool isOpen = true);
	void RenderUIQueue();
	void FinishRenderingWindow();
	void Execute();

	void RenderSpriteFontQueue(Graphics::GraphicsDevice* device);
	void RenderFullScreenQuad(Graphics::GraphicsDevice* device);

	void ClearUIClients();
	void ClearFontQueue();

	bool GetUIEnable() { return mbIsUIEnable; }

	void SetInQueue(std::string ui_name, const std::shared_ptr<UIClient>& p_client)
	{
		mUIClientsTable.emplace(ui_name, p_client);
	}
	void SetText(std::string text);

	void SetNextWindowSettings(const Vector2& pos, const Vector2& size, const Vector2& pivot = Vector2(0,0));
	void SetNextUIConfig(bool b_menubar);
	void SetNextSpriteFontSettings(const Vector2& pos, const Vector2& size, const Vector4& color);
	void SetUIEnable(bool b_enable) { mbIsUIEnable = b_enable; }

	UIRenderer(const UIRenderer&) = delete;
	UIRenderer& operator=(const UIRenderer&) = delete;
};

