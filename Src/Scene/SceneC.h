#pragma once
#include "Scene.h"
#include <memory>
#include <DirectXMath.h>

#include "./Application/Helper.h"

class ShadowMap;
class MultiRenderTarget;
class LightController;

class SceneC : public Scene
{
public:
    SceneC(SceneManager* manager, Graphics::GraphicsDevice* p_device);

    virtual void InitializeScene() override;

    virtual void Update(float elapsed_time) override;
    virtual void PreCompute(Graphics::GraphicsDevice* p_graphics) override;

    virtual void Render(Graphics::GraphicsDevice* p_graphics, float elapsed_time) override;
    virtual void RenderUI() override;

    ~SceneC();
};