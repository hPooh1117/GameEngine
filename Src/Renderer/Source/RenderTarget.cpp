#include "RenderTarget.h"

#include "DepthStencilView.h"


#include "./Utilities/misc.h"


const FLOAT RenderTarget::CLEAR_COLOR[4] = { 0.396f, 0.796f, 0.396f, 0.0f };

RenderTarget::RenderTarget()
{
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::Create(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format)
{
	D3D::Texture2DPtr texture2D = nullptr;

	D3D::RenderTargetVPtr rtv = nullptr;
	D3D::ShaderResouceVPtr srv = nullptr;
	{
		// テクスチャ作成
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = format;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		auto hr = p_device->CreateTexture2D(
			&texDesc,
			nullptr,
			texture2D.GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
	}
	//// レンダーターゲット作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	auto hr = p_device->CreateRenderTargetView(texture2D.Get(), &rtvDesc,rtv.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	// シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = p_device->CreateShaderResourceView(texture2D.Get(), &srvDesc, srv.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	mpRTVTable.emplace_back(rtv);
	mpSRVTable.emplace_back(srv);
}

void RenderTarget::Resize(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format)
{
	mpRTVTable.resize(0);
	mpSRVTable.resize(0);

	D3D::Texture2DPtr texture2D = nullptr;

	D3D::RenderTargetVPtr rtv = nullptr;
	D3D::ShaderResouceVPtr srv = nullptr;
	{
		// テクスチャ作成
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = format;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		auto hr = p_device->CreateTexture2D(
			&texDesc,
			nullptr,
			texture2D.GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
	}
	//// レンダーターゲット作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	auto hr = p_device->CreateRenderTargetView(texture2D.Get(), &rtvDesc, rtv.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	// シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = p_device->CreateShaderResourceView(texture2D.Get(), &srvDesc, srv.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	mpRTVTable.emplace_back(rtv);
	mpSRVTable.emplace_back(srv);

}

void RenderTarget::Activate(D3D::DeviceContextPtr& p_imm_context, std::unique_ptr<DepthStencilView>& p_dsv)
{
	p_imm_context->OMSetRenderTargets(mpRTVTable.size(), mpRTVTable.data()->GetAddressOf(), p_dsv->GetDSVPtr().Get());

	for (auto i = 0u; i < mpRTVTable.size(); ++i)
	{
		p_imm_context->ClearRenderTargetView(mpRTVTable.at(i).Get(), CLEAR_COLOR);
	}

	p_dsv->Clear(p_imm_context);
}

void RenderTarget::Deactivate(D3D::DeviceContextPtr& p_imm_context, unsigned int slot)
{
	p_imm_context->PSSetShaderResources(slot, mpSRVTable.size(), mpSRVTable.data()->GetAddressOf());
	p_imm_context->DSSetShaderResources(slot, mpSRVTable.size(), mpSRVTable.data()->GetAddressOf());
}
