#pragma once
#include "RenderPasses.h"

class ShadowPass :
    public RenderPass
{
public:
    ShadowPass() : RenderPass() {};
    virtual ~ShadowPass() = default;

    virtual void Initialize(D3D::DevicePtr& device) override;

    void RenderShadow();
};

