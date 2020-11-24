#pragma once
#include "Scene.h"
#include "./Renderer/D3D_Helper.h"

class ShadowMap;

class SceneF : public Scene
{
public:
public:
	SceneF(SceneManager* manager, D3D::DevicePtr& device);
	virtual void InitializeScene() override;

	virtual void Update(float elapsed_time) override;
	virtual void PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics) override;

	virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;
	~SceneF() = default;

private:
	std::shared_ptr<Actor> m_pPlayer;
	std::shared_ptr<Actor> m_pField;
	std::shared_ptr<Actor> mpSphere;
};

