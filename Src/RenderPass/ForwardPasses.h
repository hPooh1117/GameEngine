#pragma once
#include "RenderPasses.h"

#include "./Renderer/D3D_Helper.h"

class ForwardPass :
    public RenderPass
{

public:
    ForwardPass() : RenderPass() {}
    virtual ~ForwardPass() = default;

    virtual void Initialize(D3D::DevicePtr& device) override;

    void RenderForwardLighting();
};

