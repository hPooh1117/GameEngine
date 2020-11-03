#pragma once
#include "D3D_Helper.h"


class RenderTargetView
{
private:
	static const FLOAT CLEAR_COLOR[4];

private:
	D3D::RenderTargetVPtr mpRenderTargetView;

public:
	RenderTargetView(D3D::DevicePtr& p_device, DXGI_FORMAT format);
	~RenderTargetView() = default;

	void ClearRenderTargetView(D3D::DeviceContextPtr& p_imm_context);
};