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
	virtual void PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics) override;

	void         RegistUIClients();
	virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;
	virtual void RenderUI() override;

	~SceneE();

private:
	void InitializeActors();



};
