#pragma once
#include <string>
#include <map>
#include <memory>

#include "UIClient.h"

#include "./Utilities/Vector.h"

class Sprite;

class UIRenderer
{
private:
	std::map < std::string, std::shared_ptr<UIClient>> m_UIClients;
	//std::unique_ptr<Sprite> m_pSpriteFont;

public:
	UIRenderer();
	~UIRenderer() = default;

	void RenderImGui(float elapsed_time);

	void SetInQueue(std::string ui_name, const std::shared_ptr<UIClient>& p_client)
	{
		m_UIClients.emplace(ui_name, p_client);
	}

	

	UIRenderer(const UIRenderer&) = delete;
	UIRenderer& operator=(const UIRenderer&) = delete;
};

