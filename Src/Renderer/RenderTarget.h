#pragma once
#include <d3d11.h>
#include <wrl\client.h>

class RenderTarget
{
private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDSV;

public:
	RenderTarget();
	~RenderTarget();
};