#pragma once
#include "RenderPasses.h"
#include "./Renderer/ShadowMap.h"

//class ShadowMap;

class ShadowPass :
    public RenderPass
{
private:
    std::unique_ptr<ShadowMap> mpShadowMap;

public:
    ShadowPass() : RenderPass() {};
    virtual ~ShadowPass() = default;

    virtual void Initialize(Graphics::GraphicsDevice* device) override;

    void RenderShadow(Graphics::GraphicsDevice* device, float elapsed_time);
    void RenderUI(bool b_open);
};

