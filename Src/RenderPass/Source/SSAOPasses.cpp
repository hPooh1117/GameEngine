#include "SSAOPasses.h"

#include "./Engine/CameraController.h"
#include "./Engine/GameSystem.h"
#include "./Engine/MeshRenderer.h"
#include "./Engine/UIRenderer.h"

#include "./Renderer/Blur.h"
#include "./Renderer/ComputedTexture.h"
#include "./Renderer/DepthStencilView.h"
#include "./Renderer/GraphicsEngine.h"
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
	mbIsUsingCS(false)
{
}

void SSAOPass::Initialize(D3D::DevicePtr& p_device)
{
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::ESecondResult);
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::ESSAO);
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EThirdResult);
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EBlurredAO);


	if (mbIsInitialized) return;



	AddVertexAndPixelShader(p_device, ShaderID::ESSAOCompute, L"SSAO.hlsl", L"SSAO.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);
	AddVertexAndPixelShader(p_device, ShaderID::ESSAOPixel, L"SSAO.hlsl", L"SSAO.hlsl", "VSmain", "PSmain2", VEDType::VED_SPRITE);

	AddVertexAndPixelShader(p_device, ShaderID::EBlur, L"Screen.hlsl", L"Screen.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);


	mpScreen = std::make_unique<Sprite>(p_device);

	mpDSV->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT);


	mpBlurPass->Initialize(p_device);
	mpBlurPass->CreateShader(p_device);
	//mpBlurPass->ChangeSetting(1, 4);
	mpBlurPass->SetBlurStrength(1);

	if (mpAOPreparation->Initialize(p_device))
	{
		Log::Info("Initialized Ambient Occlusion.");
	}

	mbIsInitialized = true;
}

void SSAOPass::ExecuteSSAO(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{

	D3D::DeviceContextPtr& pImmContext = p_graphics->GetImmContextPtr();

	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::ESecondResult, GBUFFER_FOR_AO_MAX);

	mpAOPreparation->Activate(p_graphics, ENGINE.GetCameraPtr().get());
	if (mbIsUsingCS)
	{
		mpAOPreparation->ExecuteOcclusion(p_graphics, GetRenderTargetManager()->GetShaderResource(RenderTarget::EDepth));
		mpAOPreparation->Deactivate(p_graphics, 10);
	}

	mpScreen->RenderScreen(
		pImmContext,
		mpShaderTable.at(mbIsUsingCS ? ShaderID::ESSAOCompute : ShaderID::ESSAOPixel).get(),
		Vector2(0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT),
		Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));


	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::EThirdResult, GBUFFER_FOR_BLUR_MAX);



	mpBlurPass->ActivateBlur(pImmContext, true);
	mpBlurPass->ExecuteBlur(pImmContext, GetRenderTargetManager()->GetShaderResource(RenderTarget::ESSAO), 15);
	mpBlurPass->Deactivate(pImmContext);
	GetRenderTargetManager()->Deactivate(pImmContext, RenderTarget::EColor, DeferredPass::GEOMETRY_BUFFER_SIZE);

	mpScreen->RenderScreen(pImmContext, mpShaderTable.at(ShaderID::EBlur).get(), Vector2(0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT), Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));

	p_graphics->SetDepthStencil(GraphicsEngine::DS_TRUE);

	p_graphics->ActivateBackBuffer();

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
	//for (auto i = 0u; i < mpBlurPassTargets->GetSize(); ++i)
	//{
	//	if (ImGui::TreeNode(DeferredPass::GBUFFER_NAME[i + GBufferID::ESecondaryResult]))
	//	{
	//		if (ImGui::ImageButton((void*)mpBlurPassTargets->GetShaderResource(i).Get(), ImVec2(320, 180)))
	//		{
	//			mCurrentScreenNum = i + GBufferID::ESecondaryResult;
	//			mbIsOpen2ndScreen = true;
	//		}
	//		ImGui::TreePop();
	//	}
	//}
	//auto i = 0;
	//auto it = GetRenderTargetTable().find(RenderTarget::ESecondResult);
	//while (it != GetRenderTargetTable().end())
	//{
	//	if (ImGui::TreeNode(DeferredPass::GBUFFER_NAME[i + GBufferID::EResult]))
	//	{
	//		if (ImGui::ImageButton((void*)it->second->GetShaderResource().Get(), ImVec2(320, 180)))
	//		{
	//			mCurrentScreenNum = i + RenderTarget::ESecondResult;
	//			mbIsOpen2ndScreen = true;
	//		}
	//		ImGui::TreePop();
	//	}
	//	++i;
	//}
}

void SSAOPass::RenderUIForSettings()
{

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
