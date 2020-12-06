#pragma once
#include "Scene.h"
#include "./Renderer/D3D_Helper.h"

class ShadowMap;

class SceneF : public Scene
{
private:
	enum ActorID
	{
		EFemale,
		ESphere,
		EField,
		ACTOR_SIZE_MAX
	};



public:
	SceneF(SceneManager* manager, D3D::DevicePtr& device);
	~SceneF() = default;

	virtual void InitializeScene() override;

	virtual void Update(float elapsed_time) override;
	virtual void PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics) override;

	virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;
	virtual void RenderUI() override;


};

