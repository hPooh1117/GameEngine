#pragma once
#include <array>
#include <memory>
#include <string>
#include <vector>
#include "D3D_Helper.h"

#include "./Utilities/Log.h"
#include "GraphicsDevice.h"
#include "Shader.h"
#include "Texture.h"



class ComputedTexture
{
	struct CBufferForCompute
	{
		float param0 = 0.0f;
		float param1 = 0.0f;
		float param2 = 0.0f;
		float param3 = 0.0f;
	};

	struct CBufferForComputeInt
	{
		UINT param0 = 0;
		UINT param1 = 0;
		UINT param2 = 0;
		UINT param3 = 0;
	};

	CBufferForCompute mCBufData;
	D3D::BufferPtr mpCBuffer;
	D3D::Texture2DPtr mpTexture;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mpUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  mpSRV;
	bool mbAlreadyComputed;

	UINT mWidth, mHeight;
	UINT mLevels;

public:
	ComputedTexture();
	~ComputedTexture() = default;

	bool CreateTexture(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT level = 0);
	bool CreateTextureCube(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT level = 0);
	bool CreateTextureArray(D3D::DevicePtr& p_device, UINT width, UINT height, UINT arrayCount, DXGI_FORMAT format, UINT level = 0);
	bool CreateTexture(std::unique_ptr<Texture>& p_tex, DXGI_FORMAT format, UINT level);
	bool CreateTextureUAV(D3D::DevicePtr& p_device, UINT mip_slices);

	void SetCBuffer(D3D::DeviceContextPtr& p_imm_context, float param0 = 0, float param1 = 0, float param2 = 0, float param3 = 0);
	void SetCBuffer(D3D::DeviceContextPtr& p_imm_context, UINT param0 = 0, UINT param1 = 0, UINT param2 = 0, UINT param3 = 0);

	void Compute(Graphics::GraphicsDevice* p_device, const D3D::SRVPtr& p_srv, UINT zValue);
	void Compute(Graphics::GraphicsDevice* p_device, UINT zValue);
	void Compute(Graphics::GraphicsDevice* p_device, const D3D::SRVPtr& p_srv, UINT xValue, UINT yValue, UINT zValue);
	void Compute(Graphics::GraphicsDevice* p_device, UINT xValue, UINT yValue, UINT zValue);

	UINT NumMipLevels(UINT width, UINT height);

	void Set(D3D::DeviceContextPtr& p_imm_context, UINT slot);

	UINT  GetMipLevels() { return mLevels; }
	auto& GetSRV() { return mpSRV; }
	auto& GetTexture2D() { return mpTexture; }
};