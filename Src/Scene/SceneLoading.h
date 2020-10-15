#pragma once
#include "Scene.h"

class Sprite;

class SceneLoading : public Scene
{
private:
	std::shared_ptr<Sprite> m_pScreen;
	std::shared_ptr<Shader> m_pBasicQuad;

public:
	SceneLoading(SceneManager* p_manager, D3D::DevicePtr& p_device);
	virtual ~SceneLoading() = default;

	virtual void InitializeScene();

	virtual void Update(float elapsed_time);
	virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
};

