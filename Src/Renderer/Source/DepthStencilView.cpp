#include "DepthStencilView.h"

#include "./Application/Helper.h"
#include "./Utilities/misc.h"

bool DepthStencilView::Initialize(D3D::DevicePtr& p_device)
{
	D3D::Texture2DPtr tex2D = nullptr;
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = SCREEN_WIDTH;
	texDesc.Height = SCREEN_HEIGHT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
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

	// 深度ステンシルビュー生成
	hr = p_device->CreateDepthStencilView(
		tex2D.Get(),
		&dsvDesc,
		mpDSV.GetAddressOf()
	);
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	return true;
}

void DepthStencilView::Clear(D3D::DeviceContextPtr& p_imm_context)
{
	p_imm_context->ClearDepthStencilView(
		mpDSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);
}
