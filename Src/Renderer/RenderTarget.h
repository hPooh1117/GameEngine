#pragma once
#include <memory>
#include <vector>

#include "D3D_Helper.h"

class DepthStencilView;

class RenderTarget
{
private:
	static const FLOAT CLEAR_COLOR[4];
	

	std::vector<D3D::ShaderResouceVPtr>  mpSRVTable;
	std::vector<D3D::RenderTargetVPtr>   mpRTVTable;




public:
	RenderTarget();
	~RenderTarget();

	void Create(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format);
	void Resize(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format);
	void Activate(D3D::DeviceContextPtr& p_imm_context, std::unique_ptr<DepthStencilView>& p_dsv);
	void Deactivate(D3D::DeviceContextPtr& p_imm_context, unsigned int slot = 0);

	D3D::ShaderResouceVPtr& GetShaderResource(unsigned int slot) { return mpSRVTable.at(slot); }
	unsigned int GetSize() { return mpSRVTable.size(); }
};