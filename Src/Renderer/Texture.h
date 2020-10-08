#pragma once
#include "./Application/Helper.h"
#include "D3D_Helper.h"
#include "./Utilities/Vector.h"
class Texture
{
	enum SSMode
	{
		EWrap,
		EBorder,
	};

private:
	D3D::SamplerStatePtr m_pSamplerState = nullptr;
	D3D::ShaderResouceVPtr m_pSRV = nullptr;
	D3D::RenderTargetVPtr m_pRTV = nullptr;
public:
	D3D11_TEXTURE2D_DESC mTexDesc = {};
	
public:
	Texture();

	bool Load(D3D::DevicePtr& device, const wchar_t* filename);
	bool Load(D3D::DevicePtr& device);
	bool Create(D3D::DevicePtr& device, UINT width, UINT height, DXGI_FORMAT format, unsigned int ssmode = EBorder);

	void Set(D3D::DeviceContextPtr& imm_context, int slot = 0, bool flag = true);

	inline Vector2 GetTexSize() { return Vector2(static_cast<float>(mTexDesc.Width), static_cast<float>(mTexDesc.Height)); }
	inline auto& GetRenderTargetV() { return m_pRTV; }
	~Texture();
};

