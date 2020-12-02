#include "PostProcessPasses.h"

#include "./Engine/GameSystem.h"

#include "./Renderer/Blur.h"
#include "./Renderer/DepthStencilView.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Texture.h"
#include "./Renderer/RenderTarget.h"
#include "./Renderer/ComputedTexture.h"

#include "./Utilities/ImGuiSelf.h"
#include "./Utilities/MyArrayFromVector.h"
#include "./Utilities/Log.h"


PostProcessPass::PostProcessPass()
	:RenderPass(),
	mContrastLevel(0.0f),
	mCorrectColor(Vector3(1.0f, 1.0f, 1.0f)),
	mSaturationLevel(1.0f),
	mVignetteLevel(0.0f),
	mbIsDrawing(true),
	mbIsBlurred(true),
	mpPostProcessTex(std::make_unique<ComputedTexture>()),
	mpBlurPass(std::make_unique<BlurExecuter>())
{
}

void PostProcessPass::Initialize(D3D::DevicePtr& device)
{

	GetRenderTargetManager()->Create(device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EPostProcess);


	if (mbIsInitialized) return;

	D3D11_BUFFER_DESC cbDesc = {};
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(CBufferForPostProcess);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;

	device->CreateBuffer(&cbDesc, nullptr, mpConstantBuffer.GetAddressOf());

	mpPostProcessTex->CreateShader(device, L"./Src/Shaders/CS_Desaturate.hlsl", "CSmain");
	mpPostProcessTex->CreateTexture(device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpPostProcessTex->CreateTextureUAV(device, 0);
	mpPostProcessTex->CreateSampler(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

	mpBlurPass->Initialize(device);
	mpBlurPass->CreateShader(device);
	//mpBlurPass->ChangeSetting(1, 4);



	InitializeCommonShader(device);

	mpScreen = std::make_unique<Sprite>(device);

	AddVertexAndPixelShader(device, ShaderID::EPostEffect, L"PostEffect.hlsl", L"PostEffect.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);


	mpDSV->Create(device, SCREEN_WIDTH, SCREEN_HEIGHT);



	mbIsInitialized = true;
}

void PostProcessPass::RenderPostProcess(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	if (!mbIsDrawing) return;

	D3D::DeviceContextPtr& pImmContext = p_graphics->GetImmContextPtr();

	if (!ENGINE.IsDefferedRendering()) mCurrentRenderTarget = RenderTarget::EForward;
	else
	{
		if (ENGINE.IsSSAOActivated()) mCurrentRenderTarget = RenderTarget::EThirdResult;
		else						  mCurrentRenderTarget = RenderTarget::EFirstResult;
	}

	GetRenderTargetManager()->Deactivate(pImmContext, mCurrentRenderTarget);

	if (mbIsPostProcessed)
	{

		//const D3D::SRVPtr& pSRV = 

		// Compute Shader処理の実行
		mpPostProcessTex->SetCBuffer(pImmContext, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT));
		mpPostProcessTex->Compute(pImmContext, GetRenderTargetManager()->GetShaderResource(mCurrentRenderTarget), SCREEN_WIDTH / 32, SCREEN_HEIGHT / 16, 1);
		mpPostProcessTex->Set(pImmContext, 0);

		if (mbIsBlurred)
		{
			mpBlurPass->ActivateBlur(pImmContext, true);
			mpBlurPass->ExecuteBlur(pImmContext, mpPostProcessTex->GetSRV(), 0);
			mpBlurPass->Deactivate(pImmContext);
		}
	}


	// 最終的なスクリーンを実行
	CBufferForPostProcess cb = {};
	cb.correctColor = mCorrectColor;
	cb.contrast = mContrastLevel;
	cb.saturation = mSaturationLevel;
	cb.vignette = mVignetteLevel;
	pImmContext->UpdateSubresource(mpConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	pImmContext->PSSetConstantBuffers(1, 1, mpConstantBuffer.GetAddressOf());


	ShaderID shader = mbIsPostProcessed ? ShaderID::EPostEffect : ShaderID::ESprite;
	mpScreen->RenderScreen(pImmContext, mpShaderTable.at(shader).get(), Vector2(0.5f * ENGINE.mCurrentWidth, 0.5f * ENGINE.mCurrentHeight), Vector2(static_cast<float>(ENGINE.mCurrentWidth), static_cast<float>(ENGINE.mCurrentHeight)));
}

void PostProcessPass::RenderUI()
{
	if (mbIsPostProcessed == false) return;

	if (ImGui::TreeNode("Post Process"))
	{
		if (ImGui::ImageButton((void*)mpPostProcessTex->GetSRV().Get(), ImVec2(320, 180)))
		{

		}

		ImGui::TreePop();
	}
	mpBlurPass->RenderUI();
}

void PostProcessPass::RenderUIForSettings()
{
	ImGui::Separator();



	if (ImGui::TreeNode("Post Process"))
	{

		ImGui::Checkbox("Drawing", &mbIsDrawing);
		ImGui::Checkbox("PostProcess", &mbIsPostProcessed);
		ImGui::Checkbox("Blur", &mbIsBlurred);
		int blurStrength = mpBlurPass->GetBlurStrength();
		ImGui::SliderInt("Blur Level", &blurStrength, 0, 1);
		mpBlurPass->SetBlurStrength(blurStrength);

		MyArrayFromVector correction = MyArrayFromVector(mCorrectColor);
		ImGui::SliderFloat3("Color Correction", correction.SetArray(), 0.0f, 2.0f);
		ImGui::SliderFloat("Contrast", &mContrastLevel, -1.0f, 1.0f);
		ImGui::SliderFloat("Saturation", &mSaturationLevel, 0.0f, 1.0f);
		ImGui::SliderFloat("Vignette", &mVignetteLevel, -0.5f, 0.5f);

		ImGui::TreePop();
	}

	if (mbIsOpen2ndScreen)
	{

	}

}

