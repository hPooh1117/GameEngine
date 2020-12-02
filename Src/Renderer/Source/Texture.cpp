#include "Texture.h"
#include "ResourceManager.h"
#include "./Utilities/misc.h"


#pragma region OldImpl
////----------------------------------------------------------------------------------------------------------------------------
//
//Texture::Texture()
//{
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//bool Texture::Load(D3D::DevicePtr& device, const wchar_t* filename)
//{
//	auto hr = S_OK;
//
//	hr = ResourceManager::LoadTexFile(device, filename, mpSRV.GetAddressOf(), &mTexDesc);
//
//	//-->Create SamplerState
//	FLOAT borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
//	hr = device->CreateSamplerState(
//		&CD3D11_SAMPLER_DESC(
//			D3D11_FILTER_ANISOTROPIC,
//			D3D11_TEXTURE_ADDRESS_WRAP,
//			D3D11_TEXTURE_ADDRESS_WRAP,
//			D3D11_TEXTURE_ADDRESS_WRAP,
//			0,
//			16,
//			D3D11_COMPARISON_ALWAYS,
//			borderColor,
//			0,
//			D3D11_FLOAT32_MAX
//		),
//		m_pSamplerState.GetAddressOf()
//	);
//	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
//
//	return true;
//
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//bool Texture::Load(D3D::DevicePtr& device)
//{
//	auto hr = S_OK;
//
//	hr = ResourceManager::CreateDummyTexture(device, mpSRV);
//
//	//-->Create SamplerState
//	FLOAT borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
//	hr = device->CreateSamplerState(
//		&CD3D11_SAMPLER_DESC(
//			D3D11_FILTER_ANISOTROPIC,
//			D3D11_TEXTURE_ADDRESS_WRAP,
//			D3D11_TEXTURE_ADDRESS_WRAP,
//			D3D11_TEXTURE_ADDRESS_WRAP,
//			0,
//			16,
//			D3D11_COMPARISON_ALWAYS,
//			borderColor,
//			0,
//			D3D11_FLOAT32_MAX
//		),
//		m_pSamplerState.GetAddressOf()
//	);
//	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
//
//	return true;
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//bool Texture::Create(D3D::DevicePtr& device, UINT width, UINT height, DXGI_FORMAT format, unsigned int ssmode)
//{
//	{
//		// テクスチャ作成
//		D3D::Texture2DPtr texture2D = nullptr;
//		mTexDesc.Width = width;
//		mTexDesc.Height = height;
//		mTexDesc.MipLevels = 1;
//		mTexDesc.ArraySize = 1;
//		mTexDesc.Format = format;
//		mTexDesc.SampleDesc.Count = 1;
//		mTexDesc.Usage = D3D11_USAGE_DEFAULT;
//		mTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		mTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
//		auto hr = device->CreateTexture2D(
//			&mTexDesc,
//			nullptr,
//			texture2D.GetAddressOf()
//		);
//		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
//
//		// レンダーターゲット作成
//		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
//		rtvDesc.Format = format;
//		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
//		rtvDesc.Texture2D.MipSlice = 0;
//		hr = device->CreateRenderTargetView(texture2D.Get(), &rtvDesc, m_pRTV.GetAddressOf());
//		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
//
//		// シェーダーリソースビュー作成
//		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//		srvDesc.Format = format;
//		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//		srvDesc.Texture2D.MostDetailedMip = 0;
//		srvDesc.Texture2D.MipLevels = 1;
//		hr = device->CreateShaderResourceView(texture2D.Get(), &srvDesc, mpSRV.GetAddressOf());
//		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
//
//	}
//	if (ssmode)
//	{
//		// サンプラーステート作成
//		D3D11_SAMPLER_DESC ssDesc = {};
//		ssDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//		ssDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
//		ssDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
//		ssDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
//		ssDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//		//ボーダーカラー
//		ssDesc.BorderColor[0] = 1.0f;
//		ssDesc.BorderColor[1] = 1.0f;
//		ssDesc.BorderColor[2] = 1.0f;
//		ssDesc.BorderColor[3] = 1.0f;
//
//		ssDesc.MinLOD = 0;
//		ssDesc.MaxLOD = D3D11_FLOAT32_MAX;
//
//		auto hr = device->CreateSamplerState(&ssDesc, m_pSamplerState.GetAddressOf());
//		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
//
//	}
//	if (!ssmode)
//	{
//		//-->Create SamplerState
//		FLOAT borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
//		auto hr = device->CreateSamplerState(
//			&CD3D11_SAMPLER_DESC(
//				D3D11_FILTER_ANISOTROPIC,
//				D3D11_TEXTURE_ADDRESS_WRAP,
//				D3D11_TEXTURE_ADDRESS_WRAP,
//				D3D11_TEXTURE_ADDRESS_WRAP,
//				0,
//				16,
//				D3D11_COMPARISON_ALWAYS,
//				borderColor,
//				0,
//				D3D11_FLOAT32_MAX
//			),
//			m_pSamplerState.GetAddressOf()
//		);
//		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
//	}
//	return true;
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//void Texture::Set(D3D::DeviceContextPtr& imm_context, int slot, bool flag)
//{
//	if (!flag) // 空のテクスチャを送る
//	{
//		ID3D11ShaderResourceView* rtv[1] = { nullptr };
//		ID3D11SamplerState* ss[1] = { nullptr };
//		imm_context->PSSetShaderResources(slot, 1, rtv);
//		imm_context->PSSetSamplers(slot, 1, ss);
//
//		imm_context->DSSetShaderResources(slot, 1, rtv);
//		imm_context->DSSetSamplers(slot, 1, ss);
//		return;
//	}
//
//	//if (mpSRV == nullptr) return;
//
//	imm_context->PSSetShaderResources(slot, 1, mpSRV.GetAddressOf());
//	imm_context->PSSetSamplers(slot, 1, m_pSamplerState.GetAddressOf());
//
//	imm_context->DSSetShaderResources(slot, 1, mpSRV.GetAddressOf());
//	imm_context->DSSetSamplers(slot, 1, m_pSamplerState.GetAddressOf());
//
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//Texture::~Texture()
//{
//}
//
////----------------------------------------------------------------------------------------------------------------------------
#pragma endregion

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::Load(D3D::DevicePtr& device, const wchar_t* filename)
{
	auto hr = S_OK;

	D3D11_TEXTURE2D_DESC tex2D = {};
	hr = ResourceManager::LoadTexFile(device, filename, mpSRV.GetAddressOf(), &tex2D);
	mTextureSize.x = static_cast<float>(tex2D.Width);
	mTextureSize.y = static_cast<float>(tex2D.Height);
	return true;

}

bool Texture::Load(D3D::DevicePtr& device)
{
	auto hr = S_OK;

	hr = ResourceManager::CreateDummyTexture(device, mpSRV);
	mTextureSize.x = 1.0f;
	mTextureSize.y = 1.0f;

	//-->Create SamplerState

	return true;

}

bool Texture::Create(D3D::DevicePtr& device, UINT width, UINT height, DXGI_FORMAT format)
{
	D3D::Texture2DPtr texture2D = nullptr;

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
		auto hr = device->CreateTexture2D(
			&texDesc,
			nullptr,
			texture2D.GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
	}

		// シェーダーリソースビュー作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		auto hr = device->CreateShaderResourceView(texture2D.Get(), &srvDesc, mpSRV.GetAddressOf());
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	
	return true;

}

void Texture::Set(D3D::DeviceContextPtr& imm_context, int slot, bool flag)
{
	if (!flag) // 空のテクスチャを送る
	{
		ID3D11ShaderResourceView* rtv[1] = { nullptr };
		ID3D11SamplerState* ss[1] = { nullptr };
		imm_context->PSSetShaderResources(slot, 1, rtv);
		imm_context->DSSetShaderResources(slot, 1, rtv);
		return;
	}

	//if (mpSRV == nullptr) return;

	imm_context->PSSetShaderResources(slot, 1, mpSRV.GetAddressOf());
	imm_context->DSSetShaderResources(slot, 1, mpSRV.GetAddressOf());
}
