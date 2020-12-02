#pragma once
#include <memory>
//#include <vector>
#include <array>
#include "D3D_Helper.h"

class DepthStencilView;



class RenderTarget
{
public:
	enum RenderTargetID
	{
		EShadow,
		ECubemap,

		EForward,

		EColor,
		ENormal,
		EPosition,
		EShadowMap,
		EDepth,

		EDiffuseLight,
		ESpecularLight,
		ESkybox,
		EFirstResult,

		ESecondResult,
		ESSAO,

		EThirdResult,
		EBlurredAO,

		EPostProcess,

		RENDER_TARGET_ID_MAX,
	};
public:
	static const char* RENDER_TARGET_NAME_TABLE[RENDER_TARGET_ID_MAX];

private:
	static const FLOAT CLEAR_COLOR[4];


	std::array<D3D::RTVPtr, RENDER_TARGET_ID_MAX>  mpRTVTable;
	std::array<D3D::SRVPtr, RENDER_TARGET_ID_MAX>  mpSRVTable;


public:
	RenderTarget();
	~RenderTarget();

	void Create(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT rt_id);
	void CreateCube(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, int mip_level, UINT rt_id);

	//void Resize(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format);
	void Activate(D3D::DeviceContextPtr& p_imm_context, std::unique_ptr<DepthStencilView>& p_dsv, UINT rt_id, UINT num_activate = 1);
	void Deactivate(D3D::DeviceContextPtr& p_imm_context, UINT rt_id, UINT num_activate = 1, UINT slot = 0);
	
	void ClearAll();

	//D3D::SRVPtr& GetShaderResource(unsigned int slot) { return mpSRVTable.at(slot); }
	D3D::SRVPtr& GetShaderResource(UINT id) { return mpSRVTable[id]; }
	D3D::RTVPtr& GetRenderTarget(UINT id) { return mpRTVTable[id]; }
};