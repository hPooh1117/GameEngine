#pragma once
#include <array>
#include <memory>
#include <string>
#include <vector>
#include "D3D_Helper.h"

#include "./Utilities/Log.h"


class Shader;
class Texture;
struct ShaderMacro;

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
	D3D::SamplerStatePtr mpSampler;
	std::unique_ptr<Shader> mpCS;
	bool mbAlreadyComputed;

	UINT mWidth, mHeight;
	UINT mLevels;

public:
	ComputedTexture();
	~ComputedTexture() = default;

	bool CreateShader(D3D::DevicePtr& p_device, const wchar_t* file, const char* func);
	bool CreateShader(D3D::DevicePtr& p_device, const wchar_t* file, const char* func, const std::vector<ShaderMacro>& shader_macro);
	bool CreateTexture(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT level = 0);
	bool CreateTextureCube(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT level = 0);
	bool CreateTexture(std::unique_ptr<Texture>& p_tex, DXGI_FORMAT format, UINT level);
	bool CreateTextureUAV(D3D::DevicePtr& p_device, UINT mip_slices);
	bool CreateSampler(D3D::DevicePtr& p_device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE address_mode);

	void SetCBuffer(D3D::DeviceContextPtr& p_imm_context, float param0 = 0, float param1 = 0, float param2 = 0, float param3 = 0);
	void SetCBuffer(D3D::DeviceContextPtr& p_imm_context, UINT param0 = 0, UINT param1 = 0, UINT param2 = 0, UINT param3 = 0);

	void Compute(D3D::DeviceContextPtr& p_imm_context, const D3D::SRVPtr& p_srv, UINT zValue);
	void Compute(D3D::DeviceContextPtr& p_imm_context, UINT zValue);
	void Compute(D3D::DeviceContextPtr& p_imm_context, const D3D::SRVPtr& p_srv, UINT xValue, UINT yValue, UINT zValue);
	void Compute(D3D::DeviceContextPtr& p_imm_context, UINT xValue, UINT yValue, UINT zValue);
	void ComputeUsingCurrentSetView(D3D::DeviceContextPtr& p_imm_context, UINT xValue = 1, UINT yValue = 1, UINT zValue = 1);
	void ComputeUsingCurrentSetView(D3D::DeviceContextPtr& p_imm_context, UINT zValue = 1);

	UINT NumMipLevels(UINT width, UINT height);

	void Set(D3D::DeviceContextPtr& p_imm_context, UINT slot);

	UINT GetMipLevels() { return mLevels; }
	auto& GetSRV() { return mpSRV; }
	auto& GetTexture2D() { return mpTexture; }
};