#pragma once
#include "RenderPasses.h"

#include "./Renderer/D3D_Helper.h"


class ForwardPass : public RenderPass
{
public:
    ForwardPass();
    virtual ~ForwardPass() {}

    virtual void Initialize(Graphics::GraphicsDevice* device) override;

    void RenderForwardLighting(Graphics::GraphicsDevice* device, float elapsed_time);

    void RenderUI(bool b_open);
};

