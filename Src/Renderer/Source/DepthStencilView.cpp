#include "DepthStencilView.h"

#include "./Application/Helper.h"
#include "./Utilities/misc.h"

bool DepthStencilView::Create(D3D::DevicePtr& p_device, UINT width, UINT height)
{
	D3D::Texture2DPtr tex2D = nullptr;
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL/* | D3D11_BIND_SHADER_RESOURCE*/;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	auto hr = p_device->CreateTexture2D(
		&texDesc,
		nullptr,
		tex2D.GetAddressOf()
	);
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = texDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	//dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;

	// 深度ステンシルビュー生成
	hr = p_device->CreateDepthStencilView(
		tex2D.Get(),
		&dsvDesc,
		mpDSV.GetAddressOf()
	);
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	//D3D11_SHADER_RESOURCE_VIEW_DESC srDesc = {};
	//srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	//srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srDesc.Texture2D.MostDetailedMip = 0;
	//srDesc.Texture2D.MipLevels = -1;
	//hr = p_device->CreateShaderResourceView(tex2D.Get(), &srDesc, mpSRV.GetAddressOf());
	//_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	return true;
}

void DepthStencilView::CreateCubeDepthStencil(D3D::DevicePtr& p_device, UINT width, UINT height)
{
	auto result = S_OK;

	D3D::Texture2DPtr tex2D;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	result = p_device->CreateTexture2D(&desc, NULL, tex2D.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsDesc.Texture2DArray.FirstArraySlice = 0;
	dsDesc.Texture2DArray.ArraySize = 6;
	dsDesc.Texture2DArray.MipSlice = 0;
	dsDesc.Flags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	result = p_device->CreateDepthStencilView(tex2D.Get(), NULL, mpDSV.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));
}

void DepthStencilView::Clear(D3D::DeviceContextPtr& p_imm_context)
{
	p_imm_context->ClearDepthStencilView(
		mpDSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);
}

void DepthStencilView::Set(D3D::DeviceContextPtr& p_imm_context, UINT slot)
{
	p_imm_context->PSSetShaderResources(slot, 1, mpSRV.GetAddressOf());
	p_imm_context->DSSetShaderResources(slot, 1, mpSRV.GetAddressOf());
	p_imm_context->CSSetShaderResources(slot, 1, mpSRV.GetAddressOf());
}
