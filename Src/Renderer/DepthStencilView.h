#pragma once
#include "D3D_Helper.h"

class DepthStencilView
{
private:
	D3D::DSVPtr mpDSV;

public:
	DepthStencilView() :mpDSV(nullptr) {}
	~DepthStencilView() = default;

	bool Create(D3D::DevicePtr& p_device, UINT width, UINT height);
	void CreateCubeDepthStencil(D3D::DevicePtr& p_device, UINT width, UINT height);

	void Clear(D3D::DeviceContextPtr& p_imm_context);

	D3D::DSVPtr& GetDSVPtr() { return mpDSV; }
};