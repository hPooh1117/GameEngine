#pragma once
#include "./Application/Helper.h"
#include "D3D_Helper.h"
#include "./Utilities/Vector.h"

#pragma region OldImpl
//class Texture
//{
//	enum SSMode
//	{
//		EWrap,
//		EBorder,
//	};
//
//private:
//	D3D::SamplerStatePtr m_pSamplerState = nullptr;
//	D3D::SRVPtr mpSRV = nullptr;
//	D3D::RTVPtr m_pRTV = nullptr;
//public:
//	D3D11_TEXTURE2D_DESC mTexDesc = {};
//	
//public:
//	Texture();
//
//	bool Load(D3D::DevicePtr& device, const wchar_t* filename);
//	bool Load(D3D::DevicePtr& device);
//	bool Create(D3D::DevicePtr& device, UINT width, UINT height, DXGI_FORMAT format, unsigned int ssmode = EBorder);
//
//	void Set(D3D::DeviceContextPtr& imm_context, int slot = 0, bool flag = true);
//
//	inline Vector2 GetTexSize() { return Vector2(static_cast<float>(mTexDesc.Width), static_cast<float>(mTexDesc.Height)); }
//	inline auto& GetRenderTargetV() { return m_pRTV; }
//	inline auto& GetShaderResourceV() { return mpSRV; }
//	~Texture();
//};
#pragma endregion

class Texture
{
private:
	D3D::SRVPtr mpSRV = nullptr;
public:
	//D3D11_TEXTURE2D_DESC mTexDesc = {};
	Vector2 mTextureSize = {};
	unsigned int mID = 0;
public:
	Texture();
	~Texture();


	bool Load(D3D::DevicePtr& device, const wchar_t* filename);
	bool Load(D3D::DevicePtr& device);
	bool Create(D3D::DevicePtr& device, UINT width, UINT height, DXGI_FORMAT format);

	void Set(D3D::DeviceContextPtr& imm_context, int slot = 0, bool flag = true);

	inline Vector2 GetTexSize() { return mTextureSize; }
	inline auto& GetSRV() { return mpSRV; }
	//inline auto& GetRenderTargetV() { return m_pRTV; }
};

