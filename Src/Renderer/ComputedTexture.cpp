#include "ComputedTexture.h"

#include <cassert>
#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h"

ComputedTexture::ComputedTexture()
	:mbAlreadyComputed(false),
	mWidth(0),
	mHeight(0),
	mCBufData({}),
	mLevels(0)
{
}

//bool ComputedTexture::CreateShader(D3D::DevicePtr& p_device, const wchar_t* file, const char* func)
//{
//	mpCS->CreateComputeShader(p_device, file, func);
//	return true;
//}
//
//bool ComputedTexture::CreateShader(D3D::DevicePtr& p_device, const wchar_t* file, const char* func, const std::vector<ShaderMacro>& shader_macro)
//{
//	mpCS->CreateComputeShader(p_device, file, func, shader_macro);
//	return true;
//}

bool ComputedTexture::CreateTexture(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT level)
{
	auto result = S_OK;

	mWidth = width;
	mHeight = height;
	mLevels = level > 0 ? level : NumMipLevels(width, height);

	D3D11_TEXTURE2D_DESC t2Desc;
	ZeroMemory(&t2Desc, sizeof(t2Desc));
	t2Desc.Width = width;
	t2Desc.Height = height;
	t2Desc.MipLevels = level;
	t2Desc.ArraySize = 1;
	t2Desc.Format = format;
	t2Desc.SampleDesc.Count = 1;
	t2Desc.Usage = D3D11_USAGE_DEFAULT;
	t2Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	if (level == 0)
	{
		t2Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		t2Desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	p_device->CreateTexture2D(&t2Desc, nullptr, mpTexture.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[COMPUTED TEXTURE] Couldn't create texture 2d.");
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	p_device->CreateShaderResourceView(mpTexture.Get(), &srvDesc, mpSRV.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[COMPUTED TEXTURE] Couldn't create shader resource view.");
		return false;
	}


	return true;
}

bool ComputedTexture::CreateTextureCube(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT level)
{
	auto result = S_OK;

	mWidth = width;
	mHeight = height;
	mLevels = level > 0 ? level : NumMipLevels(width, height);


	D3D11_TEXTURE2D_DESC t2Desc;
	ZeroMemory(&t2Desc, sizeof(t2Desc));
	t2Desc.Width = width;
	t2Desc.Height = height;
	t2Desc.MipLevels = level;
	t2Desc.ArraySize = 6;
	t2Desc.Format = format;
	t2Desc.SampleDesc.Count = 1;
	t2Desc.Usage = D3D11_USAGE_DEFAULT;
	t2Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	t2Desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	if (level == 0)
	{
		t2Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		t2Desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	p_device->CreateTexture2D(&t2Desc, nullptr, mpTexture.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[COMPUTED TEXTURE] Couldn't create texture 2d.");
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	p_device->CreateShaderResourceView(mpTexture.Get(), &srvDesc, mpSRV.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[COMPUTED TEXTURE] Couldn't create shader resource view.");
		return false;
	}

	return true;
}

bool ComputedTexture::CreateTextureArray(D3D::DevicePtr& p_device, UINT width, UINT height, UINT arrayCount, DXGI_FORMAT format, UINT level)
{
	auto result = S_OK;

	mWidth = width;
	mHeight = height;
	mLevels = level > 0 ? level : NumMipLevels(width, height);


	D3D11_TEXTURE2D_DESC t2Desc;
	ZeroMemory(&t2Desc, sizeof(t2Desc));
	t2Desc.Width = width;
	t2Desc.Height = height;
	t2Desc.MipLevels = level;
	t2Desc.ArraySize = arrayCount;
	t2Desc.Format = format;
	t2Desc.SampleDesc.Count = 1;
	t2Desc.Usage = D3D11_USAGE_DEFAULT;
	t2Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;


	if (level == 0)
	{
		t2Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		t2Desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	p_device->CreateTexture2D(&t2Desc, nullptr, mpTexture.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[COMPUTED TEXTURE] Couldn't create texture 2d.");
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = arrayCount;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.MipLevels = -1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;

	p_device->CreateShaderResourceView(mpTexture.Get(), &srvDesc, mpSRV.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[COMPUTED TEXTURE] Couldn't create shader resource view.");
		return false;
	}

	return true;

}

bool ComputedTexture::CreateTexture(std::unique_ptr<Texture>& p_tex, DXGI_FORMAT format, UINT level)
{
	if (mpTexture)
	{
		mpSRV->Release();
	}


	mpSRV = p_tex->GetSRV();

	if (!mpSRV)
	{
		Log::Error("[COMPUTE TEXTURE] Couldn't create srv from unfiltered texture.");
	}

	return true;
}

bool ComputedTexture::CreateTextureUAV(D3D::DevicePtr& p_device, UINT mip_slices)
{
	assert(mpTexture);
	auto result = S_OK;

	D3D11_TEXTURE2D_DESC desc;
	mpTexture->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = desc.Format;
	if (desc.ArraySize == 1) // Texture2D�̏ꍇ
	{
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = mip_slices;
	}
	else
	{
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.MipSlice = mip_slices;
		uavDesc.Texture2DArray.FirstArraySlice = 0;
		uavDesc.Texture2DArray.ArraySize = desc.ArraySize;
	}

	result = p_device->CreateUnorderedAccessView(mpTexture.Get(), &uavDesc, mpUAV.GetAddressOf());

	if (FAILED(result))
	{
		Log::Error("[COMPUTED TEXTURE] Couldn't create unordered access view.");
		return false;
	}


	result = p_device->CreateBuffer(
		&CD3D11_BUFFER_DESC(
			sizeof(CBufferForCompute),
			D3D11_BIND_CONSTANT_BUFFER,
			D3D11_USAGE_DEFAULT,
			0,
			0,
			0
		),
		nullptr,
		mpCBuffer.GetAddressOf()
	);
	if (FAILED(result))
	{
		Log::Error("[COMPUTED TEXTURE] Couldn't Create ConstantBuffer.");
		return false;
	}


	return true;
}

//bool ComputedTexture::CreateSampler(D3D::DevicePtr& p_device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE address_mode)
//{
//	D3D11_SAMPLER_DESC sDesc;
//	ZeroMemory(&sDesc, sizeof(sDesc));
//	sDesc.Filter = filter;
//	sDesc.AddressU = address_mode;
//	sDesc.AddressV = address_mode;
//	sDesc.AddressW = address_mode;
//	sDesc.MaxAnisotropy = (filter == D3D11_FILTER_ANISOTROPIC) ? D3D11_REQ_MAXANISOTROPY : 1;
//	sDesc.MinLOD = 0;
//	sDesc.MaxLOD = D3D11_FLOAT32_MAX;
//
//	auto result = p_device->CreateSamplerState(&sDesc, mpSampler.GetAddressOf());
//	if (FAILED(result))
//	{
//		Log::Error("[COMPUTED TEXTURE] Couldn't create sampler state.");
//		return false;
//	}
//	return true;
//}

void ComputedTexture::SetCBuffer(D3D::DeviceContextPtr& p_imm_context, float param0, float param1, float param2, float param3)
{
	CBufferForCompute cb = {};
	cb.param0 = param0;
	cb.param1 = param1;
	cb.param2 = param2;
	cb.param3 = param3;

	p_imm_context->UpdateSubresource(mpCBuffer.Get(), 0, nullptr, &cb, 0, 0);
	p_imm_context->CSSetConstantBuffers(0, 1, mpCBuffer.GetAddressOf());
}

void ComputedTexture::SetCBuffer(D3D::DeviceContextPtr& p_imm_context, UINT param0, UINT param1, UINT param2, UINT param3)
{
	CBufferForComputeInt cb = {};
	cb.param0 = param0;
	cb.param1 = param1;
	cb.param2 = param2;
	cb.param3 = param3;

	p_imm_context->UpdateSubresource(mpCBuffer.Get(), 0, nullptr, &cb, 0, 0);
	p_imm_context->CSSetConstantBuffers(0, 1, mpCBuffer.GetAddressOf());
}

void ComputedTexture::Compute(Graphics::GraphicsDevice* p_device, const D3D::SRVPtr& p_srv, UINT zValue)
{
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	auto& ImmContext = p_device->GetImmContextPtr();

	//mpCS->ActivateCSShader(ImmContext);
	ImmContext->CSSetShaderResources(0, 1, p_srv.GetAddressOf());
	ImmContext->CSSetUnorderedAccessViews(0, 1, mpUAV.GetAddressOf(), nullptr);
	ImmContext->Dispatch(mWidth / 32, mHeight / 32, zValue);
	ImmContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
}

void ComputedTexture::Compute(Graphics::GraphicsDevice* p_device, UINT zValue)
{
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	auto& ImmContext = p_device->GetImmContextPtr();

	//mpCS->ActivateCSShader(ImmContext);
	ImmContext->CSSetShaderResources(0, 1, mpSRV.GetAddressOf());
	ImmContext->CSSetUnorderedAccessViews(0, 1, mpUAV.GetAddressOf(), nullptr);
	ImmContext->Dispatch(mWidth / 32, mHeight / 32, zValue);
	ImmContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

}

void ComputedTexture::Compute(Graphics::GraphicsDevice* p_device, const D3D::SRVPtr& p_srv, UINT xValue, UINT yValue, UINT zValue)
{
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	auto& ImmContext = p_device->GetImmContextPtr();

	//mpCS->ActivateCSShader(ImmContext);
	ImmContext->CSSetShaderResources(0, 1, p_srv.GetAddressOf());
	ImmContext->CSSetUnorderedAccessViews(0, 1, mpUAV.GetAddressOf(), nullptr);
	ImmContext->Dispatch(xValue, yValue, zValue);
	ImmContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

}

void ComputedTexture::Compute(Graphics::GraphicsDevice* p_device, UINT xValue, UINT yValue, UINT zValue)
{
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	auto& ImmContext = p_device->GetImmContextPtr();

	//mpCS->ActivateCSShader(ImmContext);
	ImmContext->CSSetShaderResources(0, 1, mpSRV.GetAddressOf());
	ImmContext->CSSetUnorderedAccessViews(0, 1, mpUAV.GetAddressOf(), nullptr);
	ImmContext->Dispatch(xValue, yValue, zValue);
	ImmContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

}

//void ComputedTexture::ComputeUsingCurrentSetView(Graphics::GraphicsDevice* p_device, UINT xValue, UINT yValue, UINT zValue)
//{
//	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
//	auto& ImmContext = p_device->GetImmContextPtr();
//
//	mpCS->ActivateCSShader(ImmContext);
//	ImmContext->CSSetUnorderedAccessViews(0, 1, mpUAV.GetAddressOf(), nullptr);
//	ImmContext->Dispatch(xValue, yValue, zValue);
//	ImmContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
//}
//
//void ComputedTexture::ComputeUsingCurrentSetView(Graphics::GraphicsDevice* p_device, UINT zValue)
//{
//	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
//	auto& ImmContext = p_device->GetImmContextPtr();
//
//	mpCS->ActivateCSShader(ImmContext);
//	//ImmContext->CSSetSamplers(0, 1, mpSampler.GetAddressOf());
//	ImmContext->CSSetUnorderedAccessViews(0, 1, mpUAV.GetAddressOf(), nullptr);
//	ImmContext->Dispatch(mWidth/32, mHeight/32, zValue);
//	ImmContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
//}

UINT ComputedTexture::NumMipLevels(UINT width, UINT height)
{
	UINT levels = 1;
	while ((width | height) >> levels)
	{
		++levels;
	}
	return levels;
}

void ComputedTexture::Set(D3D::DeviceContextPtr& p_imm_context, UINT slot)
{
	p_imm_context->PSSetShaderResources(slot, 1, mpSRV.GetAddressOf());
	p_imm_context->CSSetShaderResources(slot, 1, mpSRV.GetAddressOf());
}
