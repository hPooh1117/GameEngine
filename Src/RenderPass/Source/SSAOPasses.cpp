#include "SSAOPasses.h"

#include "./Engine/CameraController.h"
#include "./Engine/GameSystem.h"
#include "./Engine/MeshRenderer.h"
#include "./Engine/UIRenderer.h"

#include "./Renderer/Blur.h"
#include "./Renderer/ComputedTexture.h"
#include "./Renderer/DepthStencilView.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/RenderTarget.h"
#include "./Renderer/Shader.h"
#include "./Renderer/AmbientOcclusion.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/VertexDecleration.h"

#include "DeferredPasses.h"

#include "./Utilities/ImGuiSelf.h"
#include "./Utilities/Log.h"




SSAOPass::SSAOPass()
	:mpAOPreparation(std::make_unique<AmbientOcclusion>()),
	mpBlurPassTargets(std::make_unique<RenderTarget>()),
	mpBlurPass(std::make_unique<BlurExecuter>()),
	mbIsUsingCS(true),
	mbIsUsingAlchemyAO(false)
{
}

void SSAOPass::Initialize(Graphics::GraphicsDevice* p_device)
{
	auto pDevice = p_device->GetDevicePtr();

	GetRenderTargetManager()->Create(pDevice, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::ESecondResult);
	GetRenderTargetManager()->Create(pDevice, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::ESSAO);
	GetRenderTargetManager()->Create(pDevice, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EThirdResult);
	GetRenderTargetManager()->Create(pDevice, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EBlurredAO);


	if (mbIsInitialized) return;



	AddVertexAndPixelShader(p_device, ShaderID::ESSAOCompute, L"SSAO.hlsl", L"SSAO.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);
	AddVertexAndPixelShader(p_device, ShaderID::ESSAOPixel, L"SSAO.hlsl", L"SSAO.hlsl", "VSmain", "PSmain2", VEDType::VED_SPRITE);

	AddVertexAndPixelShader(p_device, ShaderID::EAlchemyAO, L"SSAO.hlsl", L"SSAO.hlsl", "VSmain", "PSmainAlchemy", VEDType::VED_SPRITE);

	AddVertexAndPixelShader(p_device, ShaderID::EBlur, L"Screen.hlsl", L"Screen.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);


	mpScreen = std::make_unique<Sprite>(p_device);

	mpDSV->Create(pDevice, SCREEN_WIDTH, SCREEN_HEIGHT);


	mpBlurPass->Initialize(pDevice);
	mpBlurPass->CreateShader(pDevice);
	//mpBlurPass->ChangeSetting(1, 4);
	mpBlurPass->SetBlurStrength(0);

	if (mpAOPreparation->Initialize(p_device))
	{
		Log::Info("Initialized Ambient Occlusion.");
	}

	mbIsInitialized = true;
}

void SSAOPass::ExecuteSSAO(Graphics::GraphicsDevice* p_device, float elapsed_time)
{

	D3D::DeviceContextPtr& pImmContext = p_device->GetImmContextPtr();

	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::ESecondResult, GBUFFER_FOR_AO_MAX);

	mpAOPreparation->Activate(p_device, ENGINE.GetCameraPtr().get());
	if (mbIsUsingCS)
	{
		mpAOPreparation->ExecuteOcclusion(p_device, GetRenderTargetManager()->GetShaderResource(RenderTarget::EDepth));
		mpAOPreparation->Deactivate(p_device, 10);
	}

	if (mbIsUsingAlchemyAO) 
		mpScreen->RenderScreen(
			p_device,
		mpShaderTable.at(mbIsUsingCS ? ShaderID::ESSAOCompute : ShaderID::EAlchemyAO).get(),
		Vector2(0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT),
		Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));
	else
		mpScreen->RenderScreen(
			p_device,
			mpShaderTable.at(mbIsUsingCS ? ShaderID::ESSAOCompute : ShaderID::ESSAOPixel).get(),
			Vector2(0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT),
			Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));


	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::EThirdResult, GBUFFER_FOR_BLUR_MAX);



	mpBlurPass->ActivateBlur(pImmContext, true);
	mpBlurPass->ExecuteBlur(p_device, GetRenderTargetManager()->GetShaderResource(RenderTarget::ESSAO), 15);
	mpBlurPass->Deactivate(pImmContext);
	GetRenderTargetManager()->Deactivate(pImmContext, RenderTarget::EColor, DeferredPass::GEOMETRY_BUFFER_SIZE);

	mpScreen->RenderScreen(p_device, mpShaderTable.at(ShaderID::EBlur).get(), Vector2(0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT), Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));

	p_device->OMSetDepthStencilState(Graphics::DS_TRUE);

	p_device->ActivateBackBuffer();

	GetRenderTargetManager()->Deactivate(pImmContext, RenderTarget::EThirdResult, GBUFFER_FOR_BLUR_MAX);

}

void SSAOPass::RenderUI(bool b_open)
{

	for (UINT i = 0; i < GBUFFER_FOR_AO_MAX + GBUFFER_FOR_BLUR_MAX; ++i)
	{
		if (!b_open) ImGui::SetNextItemOpen(b_open);

		if (ImGui::TreeNode(RenderTarget::RENDER_TARGET_NAME_TABLE[i + RenderTarget::ESecondResult]))
		{
			if (ImGui::ImageButton((void*)GetRenderTargetManager()->GetShaderResource(i + RenderTarget::ESecondResult).Get(), ImVec2(320, 180)))
			{
				//mCurrentScreenNum = i + RenderTarget::ESecondResult;
				//mbIsOpen2ndScreen = true;

				mChosenRenderTarget = RenderTarget::ESecondResult + i;
				mbShowsResult = false;

			}
			ImGui::TreePop();
		}
	}
}

void SSAOPass::RenderUIForSettings()
{
	//ImGui::Checkbox("AlchemyAO", &mbIsUsingAlchemyAO);

	mpAOPreparation->RenderUI();
	int blurStrength = mpBlurPass->GetBlurStrength();
	ImGui::SliderInt("Blur Level", &blurStrength, 0, 1);
	mpBlurPass->SetBlurStrength(blurStrength);
	ImGui::Checkbox("Enable Compute Shader", &mbIsUsingCS);
	
}

void SSAOPass::RenderUIForAnotherScreen()
{


	//if (mbIsOpen2ndScreen)
	//{
	//	if (mCurrentScreenNum != RenderTarget::ESSAO) return;

	//	D3D::SRVPtr srv = GetRenderTargetManager()->GetShaderResource(mCurrentScreenNum);
	//	ENGINE.GetUIRenderer()->SetNextWindowSettings(Vector2(0, SCREEN_HEIGHT - 630), Vector2(1120, 630));
	//	ImGui::Begin("Screen 2", &mbIsOpen2ndScreen);
	//	ImGui::Image((void*)srv.Get(), ImVec2(960, 540));

	//	ENGINE.GetUIRenderer()->FinishRenderingWindow();
	//}

}
