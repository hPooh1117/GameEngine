#pragma once
#include "RenderPasses.h"

class DefferedPasses : public RenderPass
{
private:

public:
	DefferedPasses() : RenderPass() {};
	virtual ~DefferedPasses() = default;

	virtual void Initialize(D3D::DevicePtr& device) override;

	void InitializeGBuffer();
};

