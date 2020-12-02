#pragma once
#include "D3D_Helper.h"

class DepthStencilView
{
private:
	D3D::DSVPtr mpDSV;
	D3D::SRVPtr mpSRV;
public:
	DepthStencilView() :mpDSV(nullptr) {}
	~DepthStencilView() = default;

	bool Create(D3D::DevicePtr& p_device, UINT width, UINT height);
	void CreateCubeDepthStencil(D3D::DevicePtr& p_device, UINT width, UINT height);

	void Clear(D3D::DeviceContextPtr& p_imm_context);
	void Set(D3D::DeviceContextPtr& p_imm_context, UINT slot);

	D3D::DSVPtr& GetDSVPtr() { return mpDSV; }
	D3D::SRVPtr& GetSRVPtr() { return mpSRV; }
};