#pragma once
#include "Scene.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Shader.h"

class SceneLoading : public Scene
{
private:
	std::shared_ptr<Sprite> mpSprScreen;
	std::unique_ptr<Sprite> mpSprGene;
	std::unique_ptr<Sprite> mpSprLoadingString;
	std::shared_ptr<Shader> m_pBasicQuad;

public:
	SceneLoading(SceneManager* p_manager, Graphics::GraphicsDevice* p_device);
	virtual ~SceneLoading() = default;

	virtual void InitializeScene();

	virtual void Update(float elapsed_time);
	virtual void PreCompute(Graphics::GraphicsDevice* p_graphics) override;

	virtual void Render(Graphics::GraphicsDevice* p_graphics, float elapsed_time);
	virtual void RenderUI() override;
};

