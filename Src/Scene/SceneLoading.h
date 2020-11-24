#pragma once
#include "Scene.h"

class Sprite;

class SceneLoading : public Scene
{
private:
	std::shared_ptr<Sprite> mpSprScreen;
	std::unique_ptr<Sprite> mpSprGene;
	std::unique_ptr<Sprite> mpSprLoadingString;
	std::shared_ptr<Shader> m_pBasicQuad;

public:
	SceneLoading(SceneManager* p_manager, D3D::DevicePtr& p_device);
	virtual ~SceneLoading() = default;

	virtual void InitializeScene();

	virtual void Update(float elapsed_time);
	virtual void PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics) override;

	virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
};

