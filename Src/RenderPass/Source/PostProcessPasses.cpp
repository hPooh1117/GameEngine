#include "PostProcessPasses.h"

#include "./Engine/GameSystem.h"

#include "./Renderer/Blur.h"
#include "./Renderer/DepthStencilView.h"
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
	mbIsDesaturated(true),
	mpPostProcessTex(std::make_unique<ComputedTexture>()),
	mpBlurPass(std::make_unique<BlurExecuter>()),
	mCurrentRenderTarget(RenderTarget::EPostProcess),
	mbIsPostProcessed(false),
	mpCS_PostProcess(std::make_unique<Shader>())
{
}

void PostProcessPass::Initialize(Graphics::GraphicsDevice* p_device)
{
	auto& pDevice = p_device->GetDevicePtr();
	GetRenderTargetManager()->Create(pDevice, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EPostProcess);

	if (!ENGINE.IsDefferedRendering()) mChosenRenderTarget = RenderTarget::EForward;
	else
	{
		if (ENGINE.IsSSAOActivated()) mChosenRenderTarget = RenderTarget::EThirdResult;
		else						  mChosenRenderTarget = RenderTarget::EFirstResult;
	}

	mbShowsResult = true;

	if (mbIsInitialized) return;

	D3D11_BUFFER_DESC cbDesc = {};
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(CBufferForPostProcess);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;

	pDevice->CreateBuffer(&cbDesc, nullptr, mpConstantBuffer.GetAddressOf());

	mpCS_PostProcess->CreateComputeShader(pDevice, L"./Src/Shaders/CS_Desaturate.hlsl", "CSmain");

	//mpPostProcessTex->CreateShader(pDevice, L"./Src/Shaders/CS_Desaturate.hlsl", "CSmain");
	mpPostProcessTex->CreateTexture(pDevice, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpPostProcessTex->CreateTextureUAV(pDevice, 0);

	mpBlurPass->Initialize(pDevice);
	mpBlurPass->CreateShader(pDevice);
	//mpBlurPass->ChangeSetting(1, 4);



	InitializeCommonShader(p_device);

	mpScreen = std::make_unique<Sprite>(p_device);

	AddVertexAndPixelShader(p_device, ShaderID::EPostEffect, L"PostEffect.hlsl", L"PostEffect.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);


	mpDSV->Create(pDevice, SCREEN_WIDTH, SCREEN_HEIGHT);



	mbIsInitialized = true;
}

void PostProcessPass::RenderPostProcess(Graphics::GraphicsDevice* p_device, float elapsed_time)
{
	if (!mbIsDrawing) return;

	D3D::DeviceContextPtr& pImmContext = p_device->GetImmContextPtr();


	ChooseCurrentRenderTarget();

	GetRenderTargetManager()->Deactivate(pImmContext, mCurrentRenderTarget);

	if (mbIsPostProcessed)
	{
		//const D3D::SRVPtr& pSRV =

		// Compute Shader�����̎��s
		if (mbIsDesaturated)
		{
			mpCS_PostProcess->ActivateCSShader(pImmContext);
			p_device->SetSamplers(Graphics::SS_LINEAR_CLAMP, 0);
			mpPostProcessTex->SetCBuffer(pImmContext, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT));
			mpPostProcessTex->Compute(p_device, GetRenderTargetManager()->GetShaderResource(mCurrentRenderTarget), SCREEN_WIDTH / 32, SCREEN_HEIGHT / 16, 1);
			mpPostProcessTex->Set(pImmContext, 0);
		}

		if (mbIsBlurred)
		{
			mpBlurPass->ActivateBlur(pImmContext, true);
			mpBlurPass->ExecuteBlur(p_device, mbIsDesaturated ? mpPostProcessTex->GetSRV() : GetRenderTargetManager()->GetShaderResource(mCurrentRenderTarget), 0);
			mpBlurPass->Deactivate(pImmContext);
		}
	}


	// �ŏI�I�ȃX�N���[�������s
	CBufferForPostProcess cb = {};
	cb.correctColor = mCorrectColor;
	cb.contrast = mContrastLevel;
	cb.saturation = mSaturationLevel;
	cb.vignette = mVignetteLevel;
	pImmContext->UpdateSubresource(mpConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	pImmContext->PSSetConstantBuffers(1, 1, mpConstantBuffer.GetAddressOf());


	UINT shader = mbIsPostProcessed ? static_cast<UINT>(ShaderID::EPostEffect) : static_cast<UINT>(ShaderID::ESprite);
	mpScreen->RenderScreen(p_device, mpShaderTable.at(shader).get(), Vector2(0.5f * ENGINE.mCurrentWidth, 0.5f * ENGINE.mCurrentHeight), Vector2(static_cast<float>(ENGINE.mCurrentWidth), static_cast<float>(ENGINE.mCurrentHeight)));
}

void PostProcessPass::RenderUI()
{
	ImGui::Checkbox("Show Final Screen", &mbShowsResult);

	if (mbIsPostProcessed == false) return;

	if (ImGui::TreeNode("Post Process"))
	{
		if (ImGui::ImageButton((void*)mpPostProcessTex->GetSRV().Get(), ImVec2(320, 180)))
		{
			mChosenRenderTarget = RenderTarget::EPostProcess;
			mbShowsResult = false;

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
		ImGui::Checkbox("Desaturate", &mbIsDesaturated);
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

void PostProcessPass::ChooseCurrentRenderTarget()
{
	if (!ENGINE.IsDefferedRendering()) mCurrentRenderTarget = RenderTarget::EForward;
	else
	{
		if (ENGINE.IsSSAOActivated()) mCurrentRenderTarget = RenderTarget::EThirdResult;
		else						  mCurrentRenderTarget = RenderTarget::EFirstResult;
	}

	if (!mbShowsResult)
	{
		mCurrentRenderTarget = mChosenRenderTarget;
	}
}
