#pragma once
#include "Scene.h"
#include <memory>
#include <DirectXMath.h>


class ShadowMap;
class AmbientOcclusion;
class MultiRenderTarget;

class SceneE : public Scene
{
public:
	SceneE(SceneManager* manager, D3D::DevicePtr& device);

	virtual void InitializeScene() override;

	virtual void Update(float elapsed_time) override;
	void         RegistUIClients();
	virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;
	~SceneE();

private:
	void InitializeActors();


private:
	std::shared_ptr<Actor> mpPlayer;
	std::shared_ptr<Actor> mpCat;
	std::shared_ptr<Actor> mpOswell;
	std::shared_ptr<Actor> mpShiba;
	std::shared_ptr<Actor> mpField;

};
