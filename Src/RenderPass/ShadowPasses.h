#pragma once
#include "RenderPasses.h"

class GraphicsEngine;
class ShadowMap;

class ShadowPass :
    public RenderPass
{
private:
    std::unique_ptr<ShadowMap> mpShadowMap;

public:
    ShadowPass() : RenderPass() {};
    virtual ~ShadowPass() = default;

    virtual void Initialize(D3D::DevicePtr& device) override;

    void RenderShadow(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
    void RenderUI();
};

