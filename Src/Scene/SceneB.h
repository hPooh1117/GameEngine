#pragma once
#include "Scene.h"
#include <memory>
#include <DirectXMath.h>

#include "./Application/Helper.h"

class ShadowMap;
class MultiRenderTarget;

struct SimpleBuffer
{
    int i;
    float f;
};

class SceneB : public Scene
{
public:
    SceneB(SceneManager* manager, Graphics::GraphicsDevice* p_device);


    virtual void InitializeScene() override;
    virtual void Update(float elapsed_time) override;
    virtual void PreCompute(Graphics::GraphicsDevice* p_graphics) override;

    virtual void Render(Graphics::GraphicsDevice* p_graphics, float elapsed_time) override;
    virtual void RenderUI() override;

    ~SceneB();
};