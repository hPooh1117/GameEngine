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
	SceneE(SceneManager* manager, Graphics::GraphicsDevice* p_device);

	virtual void InitializeScene() override;

	virtual void Update(float elapsed_time) override;
	virtual void PreCompute(Graphics::GraphicsDevice* p_graphics) override;

	virtual void Render(Graphics::GraphicsDevice* p_graphics, float elapsed_time) override;
	virtual void RenderUI() override;

	~SceneE();

private:
	void InitializeActors();



};
