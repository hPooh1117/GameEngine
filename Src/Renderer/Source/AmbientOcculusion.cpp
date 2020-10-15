#include "AmbientOcculusion.h"

#include "GraphicsEngine.h"
#include "Texture.h"

#include "./Application/Helper.h"

#include "./Engine/DirectionalLight.h"
#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"

#include "./Utilities/misc.h"
#include "./Utilities/Util.h"

const Vector2 AmbientOcclusion::NOISE_TEX_RESOLUTION = { 8, 8 };

AmbientOcclusion::AmbientOcclusion(D3D::DevicePtr& p_device)
	:mNoiseScale(
		Vector2(
			static_cast<float>(SCREEN_WIDTH) / NOISE_TEX_RESOLUTION.x, 
			static_cast<float>(SCREEN_HEIGHT) / NOISE_TEX_RESOLUTION.y)),
	mSampleRadius(SAMPLE_RADIUS),
	mPower(1.2f),
	mBias(0.0f)
{
	//m_pAOTex = std::make_shared<Texture>();
	//m_pAOTex->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16_FLOAT);
	//
	//m_pRTV = m_pAOTex->GetRenderTargetV();
	//{
	//	//深度ステンシル設定
	//	D3D::Texture2DPtr texture2D = nullptr;
	//	D3D11_TEXTURE2D_DESC texDesc = {};
	//	texDesc.Width = SCREEN_WIDTH;
	//	texDesc.Height = SCREEN_HEIGHT;
	//	texDesc.MipLevels = 1;
	//	texDesc.ArraySize = 1;
	//	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	texDesc.SampleDesc.Count = 1;
	//	texDesc.SampleDesc.Quality = 0;
	//	texDesc.Usage = D3D11_USAGE_DEFAULT;
	//	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//	texDesc.CPUAccessFlags = 0;
	//	texDesc.MiscFlags = 0;
	//
	//	auto result = p_device->CreateTexture2D(
	//		&texDesc,
	//		nullptr,
	//		texture2D.GetAddressOf()
	//	);
	//	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));
	//
	//	// 深度ステンシルビュー
	//	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	//	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	//	dsvDesc.Format = texDesc.Format;
	//	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//	dsvDesc.Texture2D.MipSlice = 0;
	//
	//	result = p_device->CreateDepthStencilView(
	//		texture2D.Get(),
	//		&dsvDesc,
	//		m_pDSV.GetAddressOf()
	//	);
	//	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));
	//
	//}

	D3D11_BUFFER_DESC cbDesc = {};
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(CBufferForAO);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;

	p_device->CreateBuffer(&cbDesc, nullptr, m_pCBufferForAO.GetAddressOf());

	
	// SSAOの環境遮蔽係数を算出するために使用するサンプル点群の生成
	//for (int i = 0; i < MAX_SAMPLES; ++i)
	//{
	//	float r = SAMPLE_RADIUS * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	//	float t = DirectX::XM_2PI * static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) + 1.0f);
	//	float cp = 2.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 1.0f;
	//	float sp = sqrtf(1.0f - cp * cp);
	//	float cost = cosf(t), sint = sinf(t);

	//	mSamplePos[i].x = r * sp * cost;
	//	mSamplePos[i].y = r * sp * sint;
	//	mSamplePos[i].z = r * cp;
	//	mSamplePos[i].w = 0.0f;
	//}

	// 20201004
	for (int i = 0; i < MAX_SAMPLES; ++i)
	{
		mSamplePos[i].x = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 2.0f;
		mSamplePos[i].y = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 2.0f;
		mSamplePos[i].z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		mSamplePos[i].w = 0.0f;

		float scale = static_cast<float>(i) / static_cast<float>(MAX_SAMPLES);
		float scaleMul(Math::Lerp(0.1f, 1.0f, scale * scale));

		mSamplePos[i].x *= scaleMul;
		mSamplePos[i].y *= scaleMul;
		mSamplePos[i].z *= scaleMul;
	}


	
	// http://alextardif.com/SSAO.html
	float noiseTextureFloats[192];
	for (int i = 0; i < 64; ++i)
	{
		int index = i * 3;
		noiseTextureFloats[index]     = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 2.0f;
		noiseTextureFloats[index + 1] = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 2.0f;
		noiseTextureFloats[index + 2] = 0.0f;
	}

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = noiseTextureFloats;
	data.SysMemPitch = 8 * 12;
	data.SysMemSlicePitch = 0;

	D3D11_TEXTURE2D_DESC descTex = {};
	descTex.Width = NOISE_TEX_RESOLUTION.x;
	descTex.Height = NOISE_TEX_RESOLUTION.y;
	descTex.MipLevels = descTex.ArraySize = 1;
	descTex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descTex.SampleDesc.Count = 1;
	descTex.SampleDesc.Quality = 0;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	descTex.CPUAccessFlags = 0;
	descTex.MiscFlags = 0;
	auto hr = p_device->CreateTexture2D(&descTex, &data, m_pNoiseTexture.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	descSRV.Format = descTex.Format;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2D.MostDetailedMip = 0;
	descSRV.Texture2D.MipLevels = 1;
	hr = p_device->CreateShaderResourceView(m_pNoiseTexture.Get(), &descSRV, m_pNoiseResourceView.GetAddressOf());

	//-->Create SamplerState
	FLOAT borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	hr = p_device->CreateSamplerState(
		&CD3D11_SAMPLER_DESC(
			D3D11_FILTER_MIN_MAG_MIP_POINT,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			//D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			0,
			16,
			D3D11_COMPARISON_ALWAYS,
			borderColor,
			0,
			D3D11_FLOAT32_MAX
		),
		m_pWrapSampler.GetAddressOf()
	);
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
}

void AmbientOcclusion::Activate(std::unique_ptr<GraphicsEngine>& p_graphics, std::shared_ptr<CameraController>& p_camera)
{
	D3D::DeviceContextPtr immContext = p_graphics->GetImmContextPtr();

	//immContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), m_pDSV.Get());
	//float clearColor[4] = { 1, 1, 1, 1 };
	//immContext->ClearRenderTargetView(m_pRTV.Get(), clearColor);
	//immContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//p_graphics->SetViewport(SCREEN_WIDTH, SCREEN_HEIGHT);

	CBufferForAO cb = {};
	DirectX::XMStoreFloat4x4(&cb.invProj, p_camera->GetInvProjMatrix(immContext));
	DirectX::XMStoreFloat4x4(&cb.invView, p_camera->GetInvViewMatrix());
	DirectX::XMStoreFloat4x4(&cb.proj, p_camera->GetProjMatrix(immContext));
	DirectX::XMStoreFloat4x4(&cb.view, p_camera->GetInvProjViewMatrix(immContext));
	cb.screenSize = DirectX::XMFLOAT2(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT));
	cb.noiseScale = DirectX::XMFLOAT2(mNoiseScale.x, mNoiseScale.y);
	cb.kernelSize = static_cast<float>(MAX_SAMPLES);
	cb.ambientBias = mBias;
	cb.radius = mSampleRadius;
	cb.power = mPower;
	memcpy( cb.samplePos, mSamplePos, sizeof(DirectX::XMFLOAT4) * MAX_SAMPLES );

	immContext->UpdateSubresource(m_pCBufferForAO.Get(), 0, nullptr, &cb, 0, 0);
	immContext->VSSetConstantBuffers(5, 1, m_pCBufferForAO.GetAddressOf());
	immContext->PSSetConstantBuffers(5, 1, m_pCBufferForAO.GetAddressOf());

	immContext->PSSetShaderResources(8, 1, m_pNoiseResourceView.GetAddressOf());
	immContext->PSSetSamplers(8, 1, m_pWrapSampler.GetAddressOf());
}

void AmbientOcclusion::Deactivate(std::unique_ptr<GraphicsEngine>& p_graphics)
{
}

void AmbientOcclusion::RenderUI()
{
	using namespace ImGui;
	SliderFloat("Intensity", &mPower, 0.0f, 5.0f);

	SliderFloat("Sample Radius", &mSampleRadius, 0.1f, 5.0f);

	SliderFloat("Ambient Bias", &mBias, 0.0f, 5.0f);
}

AmbientOcclusion::~AmbientOcclusion()
{
}
