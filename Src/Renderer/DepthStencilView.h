#pragma once
#include "D3D_Helper.h"

class DepthStencilView
{
private:
	D3D::DepthStencilVPtr mpDSV;

public:
	DepthStencilView() :mpDSV(nullptr) {}
	~DepthStencilView() = default;

	bool Initialize(D3D::DevicePtr& p_device);
	void Clear(D3D::DeviceContextPtr& p_imm_context);

	D3D::DepthStencilVPtr& GetDSVPtr() { return mpDSV; }
};