#pragma once
#include "RenderPasses.h"

#include "./Renderer/D3D_Helper.h"

class GraphicsEngine;

class ForwardPass : public RenderPass
{
public:
    ForwardPass();
    virtual ~ForwardPass() {}

    virtual void Initialize(D3D::DevicePtr& device) override;

    void RenderForwardLighting(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);

    void RenderUI(bool b_open);
};

