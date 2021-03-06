#include "ShadowMap.h"
//#include "GraphicsDevice.h"
//#include "Shader.h"
//#include "Texture.h"

#include "./Engine/OrthoView.h"
//#include "./Engine/CameraController.h"
//#include "./Engine/DirectionalLight.h"
//#include "./Engine/LightController.h"
#include "./Application/Helper.h"

#include "./Utilities/misc.h"
#include "./Utilities/ImguiSelf.h"
#include "./Utilities/Util.h"

using namespace DirectX;

ShadowMap::ShadowMap(Graphics::GraphicsDevice* device)
	:mDistanceToLight(-40.0f)
{
	//m_pShadowTex = std::make_unique<Texture>();
	//m_pShadowTex->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	//m_pRTVShadowMap = m_pShadowTex->GetRenderTargetV();

	//{
	//	// 深度ステンシル設定
	//	D3D11_TEXTURE2D_DESC texDesc = {};
	//	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	//	texDesc.Width = SCREEN_WIDTH;
	//	texDesc.Height = SCREEN_HEIGHT;
	//	texDesc.MipLevels = 1;
	//	texDesc.ArraySize = 1;
	//	//texDesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	//	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	texDesc.SampleDesc.Count = 1;
	//	texDesc.SampleDesc.Quality = 0;
	//	texDesc.Usage = D3D11_USAGE_DEFAULT;
	//	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//	texDesc.CPUAccessFlags = 0;
	//	texDesc.MiscFlags = 0;

	//	// 深度ステンシルテクスチャ生成
	//	auto hr = p_device->CreateTexture2D(
	//		&texDesc,
	//		nullptr,
	//		m_pShadowMapDepth.GetAddressOf()
	//	);
	//	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	//	// 深度ステンシルビュー
	//	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	//	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	//	dsvDesc.Format = texDesc.Format;
	//	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//	dsvDesc.Texture2D.MipSlice = 0;

	//	// 深度ステンシルビュー生成
	//	hr = p_device->CreateDepthStencilView(
	//		m_pShadowMapDepth.Get(),
	//		&dsvDesc,
	//		m_pDSVShadowMap.GetAddressOf()
	//	);
	//	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
	//}

	D3D11_BUFFER_DESC cbDesc = {};
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(CBufferForShadow);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;

	device->GetDevicePtr()->CreateBuffer(&cbDesc, nullptr, m_pCBufferForShadow.GetAddressOf());

}

//void ShadowMap::Activate(
//	std::unique_ptr<GraphicsDevice>& p_graphics,
//	std::shared_ptr<Light>& p_light,
//	std::shared_ptr<CameraController>& p_camera)
//{
//	D3D::DeviceContextPtr& immContext = p_graphics->GetImmContextPtr();
//
//	float clearColor[4] = { 1,1,1,1 };
//	immContext->ClearRenderTargetView(m_pRTVShadowMap.Get(), clearColor);
//	immContext->ClearDepthStencilView(
//		m_pDSVShadowMap.Get(),
//		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
//		1.0f, 0);
//
//	// レンダーターゲットビュー設定
//	immContext->OMSetRenderTargets(
//		1,
//		m_pRTVShadowMap.GetAddressOf(),
//		m_pDSVShadowMap.Get());
//
//	p_graphics->RSSetViewports(SCREEN_WIDTH, SCREEN_HEIGHT);
//
//
//	Vector3 lightDir = std::static_pointer_cast<DirectionalLight>(p_light)->GetLightDirection().getXYZ();
//	p_camera->SetOrthoPos(-40.0f * lightDir);
//	XMStoreFloat4x4(&mLightViewProj, p_camera->GetOrthoView() * p_camera->GetOrthoProj(immContext));
//
//	CBufferForShadow cb = {};
//	cb.shadowVP = mLightViewProj;
//	immContext->UpdateSubresource(m_pCBufferForShadow.Get(), 0, nullptr, &cb, 0, 0);
//	immContext->VSSetConstantBuffers(6, 1, m_pCBufferForShadow.GetAddressOf());
//	immContext->PSSetConstantBuffers(6, 1, m_pCBufferForShadow.GetAddressOf());
//}

void ShadowMap::Activate(
	Graphics::GraphicsDevice* device,
	LightController* p_light,
	CameraController* p_camera)
{
	D3D::DeviceContextPtr& immContext = device->GetImmContextPtr();

	//float clearColor[4] = { 1,1,1,1 };
	//immContext->ClearRenderTargetView(m_pRTVShadowMap.Get(), clearColor);
	//immContext->ClearDepthStencilView(
	//	m_pDSVShadowMap.Get(),
	//	D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
	//	1.0f, 0);

	//// レンダーターゲットビュー設定
	//immContext->OMSetRenderTargets(
	//	1,
	//	m_pRTVShadowMap.GetAddressOf(),
	//	m_pDSVShadowMap.Get());

	//p_graphics->RSSetViewports(SCREEN_WIDTH, SCREEN_HEIGHT);

	Vector4 light = p_light->GetLightDirection();
	Vector3 lightDir = light.getXYZ();

	p_camera->SetOrthoPos(mDistanceToLight * lightDir);
	XMStoreFloat4x4(&mLightViewProj, p_camera->GetOrthoView() * p_camera->GetOrthoProj(immContext));

	CBufferForShadow cb = {};
	cb.shadowVP = mLightViewProj;
	immContext->UpdateSubresource(m_pCBufferForShadow.Get(), 0, nullptr, &cb, 0, 0);
	immContext->VSSetConstantBuffers(6, 1, m_pCBufferForShadow.GetAddressOf());
	immContext->PSSetConstantBuffers(6, 1, m_pCBufferForShadow.GetAddressOf());

}

void ShadowMap::Deactivate(Graphics::GraphicsDevice* device)
{
	//p_graphics->ActivateBackBuffer();

	//m_pShadowTex->Set(p_graphics->GetImmContextPtr(), 5);
}

void ShadowMap::RenderUI()
{
	using namespace ImGui;
	Text("Shadow");

	SliderFloat("Distance to light", &mDistanceToLight, -100.0f, -30.0f);


	//Image((void*)m_pShadowTex->GetShaderResourceV().Get(), ImVec2(320, 180)/*, ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT)*/);
	
}


ShadowMap::~ShadowMap()
{
}
