#include "MultiRenderTarget.h"

#include "Texture.h"
#include "Sprite.h"
#include "Shader.h"
#include "GraphicsEngine.h"
#include "./Utilities/misc.h"
#include "Blender.h"


//----------------------------------------------------------------------------------------------------------------------------

MultiRenderTarget::MultiRenderTarget(D3D::DevicePtr& p_device):mCurrentType(GBufferType::kResult)
{
	//
	// テクスチャ初期化
	//
	m_pColorMap = std::make_unique<Texture>();
	m_pColorMap->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	m_pNormalMap = std::make_unique<Texture>();
	m_pNormalMap->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	m_pPositionMap = std::make_unique<Texture>();
	m_pPositionMap->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	m_pShadowMap = std::make_unique<Texture>();
	m_pShadowMap->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	m_pDepthMap = std::make_unique<Texture>();
	m_pDepthMap->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);



	m_pLightMap = std::make_unique<Texture>();
	m_pLightMap->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_pDiffuseLightMap = std::make_unique<Texture>();
	m_pDiffuseLightMap->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_pSpecularLightMap = std::make_unique<Texture>();
	m_pSpecularLightMap->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_pAmbientLightMap = std::make_unique<Texture>();
	m_pAmbientLightMap->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_pScreenMap = std::make_unique<Texture>();
	m_pScreenMap->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_pColor = std::make_unique<Sprite>(p_device);


	m_pRTVs.at(0) = m_pColorMap->GetRenderTargetV();
	m_pRTVs.at(1) = m_pNormalMap->GetRenderTargetV();
	m_pRTVs.at(2) = m_pPositionMap->GetRenderTargetV();
	m_pRTVs.at(3) = m_pShadowMap->GetRenderTargetV();
	m_pRTVs.at(4) = m_pDepthMap->GetRenderTargetV();

	m_pRTVForLight = m_pLightMap->GetRenderTargetV();

	m_pRTVsForLights.at(0) = m_pDiffuseLightMap->GetRenderTargetV();
	m_pRTVsForLights.at(1) = m_pSpecularLightMap->GetRenderTargetV();
	m_pRTVsForLights.at(2) = m_pAmbientLightMap->GetRenderTargetV();

	m_pRTVForScreen = m_pScreenMap->GetRenderTargetV();
	{
		// 深度ステンシルビュー作成
		// 深度ステンシル設定
		D3D11_TEXTURE2D_DESC texDesc = {};
		ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width = SCREEN_WIDTH;
		texDesc.Height = SCREEN_HEIGHT;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		//texDesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
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
			m_pTexForDSV.GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

		// 深度ステンシルビュー
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvDesc.Format = texDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		// 深度ステンシルビュー生成
		hr = p_device->CreateDepthStencilView(
			m_pTexForDSV.Get(),
			&dsvDesc,
			m_pDSV.GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
	}
}

//----------------------------------------------------------------------------------------------------------------------------

void MultiRenderTarget::ActivateGBuffer(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	D3D::DeviceContextPtr immContext = p_graphics->GetImmContextPtr();

	immContext->OMSetRenderTargets(NUMBER_OF_GBUFFER, m_pRTVs.data()->GetAddressOf(), m_pDSV.Get());
	float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	for (int i = 0; i < NUMBER_OF_GBUFFER; ++i)
	{
		immContext->ClearRenderTargetView(m_pRTVs.at(i).Get(), clearColor);
	}

	immContext->ClearDepthStencilView(
		m_pDSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);

	p_graphics->SetViewport(SCREEN_WIDTH, SCREEN_HEIGHT);

}

//----------------------------------------------------------------------------------------------------------------------------

void MultiRenderTarget::ActivateDefferedLight(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	D3D::DeviceContextPtr immContext = p_graphics->GetImmContextPtr();

	immContext->OMSetRenderTargets(NUMBER_OF_LIGHTMAP, m_pRTVsForLights.data()->GetAddressOf(), m_pDSV.Get());
	float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	for (int i = 0; i < NUMBER_OF_LIGHTMAP; ++i)
	{
		immContext->ClearRenderTargetView(m_pRTVsForLights.at(i).Get(), clearColor);
	}
	//immContext->OMSetRenderTargets(1, m_pRTVForLight.GetAddressOf(), m_pDSV.Get());
	//float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	//immContext->ClearRenderTargetView(m_pRTVForLight.Get(), clearColor);

	immContext->ClearDepthStencilView(
		m_pDSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);

	p_graphics->SetViewport(SCREEN_WIDTH, SCREEN_HEIGHT);

	m_pNormalMap->Set(immContext, 1);
	m_pPositionMap->Set(immContext, 3);
	//m_pShadowMap->Set(immContext, 5);
	m_pDepthMap->Set(immContext, 7);

	float width = static_cast<float>(SCREEN_WIDTH);
	float height = static_cast<float>(SCREEN_HEIGHT);

	Vector2 pos(width * 0.5f, height * 0.5f);
	Vector2 size(width, height);
	Vector2 texPos(0, 0);

	float angle = 0.0f;
	Vector4 color(1, 1, 1, 1);

	m_pColor->Render(
		immContext,
		m_pShaders.at(1),
		m_pColorMap,
		pos,
		size,
		texPos,
		size, angle, color
	);

}

//----------------------------------------------------------------------------------------------------------------------------

void MultiRenderTarget::ActivateScreen(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	D3D::DeviceContextPtr immContext = p_graphics->GetImmContextPtr();

	immContext->OMSetRenderTargets(1, m_pRTVForScreen.GetAddressOf(), m_pDSV.Get());
	float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	immContext->ClearRenderTargetView(m_pRTVForScreen.Get(), clearColor);

	immContext->ClearDepthStencilView(
		m_pDSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);

	p_graphics->SetViewport(SCREEN_WIDTH, SCREEN_HEIGHT);

}

//----------------------------------------------------------------------------------------------------------------------------

void MultiRenderTarget::Deactivate(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	p_graphics->ActivateRenderTarget();
}

//----------------------------------------------------------------------------------------------------------------------------

void MultiRenderTarget::Render(
	D3D::DeviceContextPtr& imm_context,
	std::unique_ptr<Blender>& p_blender)
{
	float width = static_cast<float>(SCREEN_WIDTH);
	float height = static_cast<float>(SCREEN_HEIGHT);

	Vector2 pos(width * 0.5f, height * 0.5f);
	Vector2 size(width, height);
	Vector2 texPos(0, 0);

	float angle = 0.0f;
	Vector4 color(1, 1, 1, 1);

	switch (mCurrentType)
	{
	case GBufferType::kAlbedo:
		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pColorMap,
			pos,
			size,
			texPos,
			size, angle, color
		);
		break;

	case GBufferType::kPosition:
		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pPositionMap,
			pos,
			size,
			texPos,
			size, angle, color
		);
		break;

	case GBufferType::kNormal:
		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pNormalMap,
			pos,
			size,
			texPos,
			size, angle, color
		);
		break;

	case GBufferType::kDepth:
		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pDepthMap,
			pos,
			size,
			texPos,
			size, angle, color
		);
		break;

	case GBufferType::kShadow:
		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pShadowMap,
			pos,
			size,
			texPos,
			size, angle, color
		);
		break;

	case GBufferType::kDiffuse:
		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pDiffuseLightMap,
			pos,
			size,
			texPos,
			size, angle, color
		);
		break;

	case GBufferType::kSpecular:
		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pSpecularLightMap,
			pos,
			size,
			texPos,
			size, angle, color
		);
		break;

	case GBufferType::kAmbient:
		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pAmbientLightMap,
			pos,
			size,
			texPos,
			size, angle, color
		);
		break;

	case GBufferType::kResult:
		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pColorMap,
			pos,
			size,
			texPos,
			size, angle, color
		);

		p_blender->SetBlendState(imm_context, Blender::BLEND_MULTIPLY);

		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pShadowMap,
			pos,
			size,
			texPos,
			size, angle, color
		);

		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pAmbientLightMap,
			pos,
			size,
			texPos,
			size, angle, color
		);

		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pDiffuseLightMap,
			pos,
			size,
			texPos,
			size, angle, color
		);

		p_blender->SetBlendState(imm_context, Blender::BLEND_ADD);

		m_pColor->Render(
			imm_context,
			m_pShaders.at(0),
			m_pSpecularLightMap,
			pos,
			size,
			texPos,
			size, angle, color
		);
		break;
	}


}

//----------------------------------------------------------------------------------------------------------------------------

void MultiRenderTarget::RenderScreen(D3D::DeviceContextPtr& imm_context, std::unique_ptr<Blender>& p_blender)
{
	float width = static_cast<float>(SCREEN_WIDTH);
	float height = static_cast<float>(SCREEN_HEIGHT);

	Vector2 pos(width * 0.5f, height * 0.5f);
	Vector2 size(width, height);
	Vector2 texPos(0, 0);

	float angle = 0.0f;
	Vector4 color(1, 1, 1, 1);

	// 画像の合成
	m_pColor->Render(
		imm_context,
		m_pShaders.at(0),
		m_pScreenMap,
		pos,
		size,
		texPos,
		size, angle, color
	);


}

void MultiRenderTarget::RenderUI()
{
	using namespace ImGui;
	
	RadioButton("Albedo",         &mCurrentType,       GBufferType::kAlbedo     );         
	RadioButton("Position",       &mCurrentType,       GBufferType::kPosition   );       
	RadioButton("Normal",         &mCurrentType,       GBufferType::kNormal     );         
	RadioButton("Depth",          &mCurrentType,       GBufferType::kDepth      );          
	RadioButton("Shadow",         &mCurrentType,       GBufferType::kShadow     );         
	RadioButton("Diffuse Light",  &mCurrentType,       GBufferType::kDiffuse    );         
	RadioButton("Specular Light", &mCurrentType,       GBufferType::kSpecular   );       
	RadioButton("Ambient",        &mCurrentType,       GBufferType::kAmbient    );               
	RadioButton("Result",         &mCurrentType,       GBufferType::kResult     );                 
}

//----------------------------------------------------------------------------------------------------------------------------


MultiRenderTarget::~MultiRenderTarget()
{
}

//----------------------------------------------------------------------------------------------------------------------------
