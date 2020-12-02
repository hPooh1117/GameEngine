#include "RenderTarget.h"

#include "DepthStencilView.h"


#include "./Utilities/misc.h"
#include "./Utilities/Log.h"


const FLOAT RenderTarget::CLEAR_COLOR[4] = { 0.2f, 0.2f, 0.2f, 0.0f };
const char* RenderTarget::RENDER_TARGET_NAME_TABLE[RENDER_TARGET_ID_MAX] = {
	"Shadow",
	"Cubemap",
	"Forward",
	"Color",
	"Normal",
	"Position",
	"ShadowMap",
	"Depth",
	"Diffuse Light",
	"Specular Light",
	"Skybox Deffered",
	"Lighting Result",
	"AO + Lighting",
	"SSAO",
	"BluredAO + Lighting",
	"BluredAO",
	"PostProcess"
};

RenderTarget::RenderTarget()
{
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::Create(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT rt_id)
{
	D3D::Texture2DPtr texture2D = nullptr;

	{
		// テクスチャ作成
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 0;
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
	auto hr = p_device->CreateRenderTargetView(texture2D.Get(), &rtvDesc, mpRTVTable[rt_id].GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	// シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = p_device->CreateShaderResourceView(texture2D.Get(), &srvDesc, mpSRVTable[rt_id].GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	Log::Info("Created GBuffer.(%s)", RENDER_TARGET_NAME_TABLE[rt_id]);
}

void RenderTarget::CreateCube(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, int mip_level, UINT rt_id)
{
	auto result = S_OK;

	D3D::Texture2DPtr      tex2D;


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = mip_level;
	desc.ArraySize = 6;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	result = p_device->CreateTexture2D(&desc, NULL, tex2D.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory(&srDesc, sizeof(srDesc));
	srDesc.Format = format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srDesc.TextureCube.MostDetailedMip = 0;
	srDesc.TextureCube.MipLevels = mip_level;

	result = p_device->CreateShaderResourceView(tex2D.Get(), &srDesc, mpSRVTable[rt_id].GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	ZeroMemory(&rtDesc, sizeof(rtDesc));
	rtDesc.Format = format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtDesc.Texture2DArray.FirstArraySlice = 0;
	rtDesc.Texture2DArray.MipSlice = 0;
	rtDesc.Texture2DArray.ArraySize = 6;
	result = p_device->CreateRenderTargetView(tex2D.Get(), &rtDesc, mpRTVTable[rt_id].GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));
}


//void RenderTarget::Resize(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format)
//{
//	//mpRTVTable.resize(0);
//	//mpSRVTable.resize(0);
//
//	D3D::Texture2DPtr texture2D = nullptr;
//
//	D3D::RTVPtr rtv = nullptr;
//	D3D::SRVPtr srv = nullptr;
//	{
//		// テクスチャ作成
//		D3D11_TEXTURE2D_DESC texDesc = {};
//		texDesc.Width = width;
//		texDesc.Height = height;
//		texDesc.MipLevels = 1;
//		texDesc.ArraySize = 1;
//		texDesc.Format = format;
//		texDesc.SampleDesc.Count = 1;
//		texDesc.Usage = D3D11_USAGE_DEFAULT;
//		texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
//		auto hr = p_device->CreateTexture2D(
//			&texDesc,
//			nullptr,
//			texture2D.GetAddressOf()
//		);
//		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
//	}
//	//// レンダーターゲット作成
//	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
//	rtvDesc.Format = format;
//	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
//	rtvDesc.Texture2D.MipSlice = 0;
//	auto hr = p_device->CreateRenderTargetView(texture2D.Get(), &rtvDesc, rtv.GetAddressOf());
//	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
//
//	// シェーダーリソースビュー作成
//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//	srvDesc.Format = format;
//	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MostDetailedMip = 0;
//	srvDesc.Texture2D.MipLevels = 1;
//	hr = p_device->CreateShaderResourceView(texture2D.Get(), &srvDesc, srv.GetAddressOf());
//	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
//
//
//	mpSRV.Reset();
//	mpRTV.Reset();
//	mpSRV = srv;
//	mpRTV = rtv;
//}

void RenderTarget::Activate(D3D::DeviceContextPtr& p_imm_context, std::unique_ptr<DepthStencilView>& p_dsv, UINT start_id, UINT num_activate)
{
	p_imm_context->OMSetRenderTargets(num_activate, mpRTVTable[start_id].GetAddressOf(), p_dsv->GetDSVPtr().Get());

	for (auto i = 0; i < num_activate; ++i)
	{
		p_imm_context->ClearRenderTargetView(mpRTVTable.at(start_id + i).Get(), CLEAR_COLOR);
	}

	p_dsv->Clear(p_imm_context);

}

void RenderTarget::Deactivate(D3D::DeviceContextPtr& p_imm_context, UINT start_id, UINT num_activate, UINT slot)
{
	p_imm_context->PSSetShaderResources(slot, num_activate, mpSRVTable[start_id].GetAddressOf());
	p_imm_context->DSSetShaderResources(slot, num_activate, mpSRVTable[start_id].GetAddressOf());
	p_imm_context->CSSetShaderResources(slot, num_activate, mpSRVTable[start_id].GetAddressOf());
}

void RenderTarget::ClearAll()
{
	for (auto i = 0; i < RENDER_TARGET_ID_MAX; ++i)
	{
		if (mpRTVTable[i] != nullptr)
		{
			mpRTVTable[i].Reset();
		}
		if (mpSRVTable[i] != nullptr) mpSRVTable[i].Reset();
	}
	Log::Info("Render Targets are empty.");
}

