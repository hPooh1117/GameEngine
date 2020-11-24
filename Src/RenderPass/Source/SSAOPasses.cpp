#include "SSAOPasses.h"

#include "./Engine/CameraController.h"
#include "./Engine/GameSystem.h"
#include "./Engine/MeshRenderer.h"
#include "./Engine/UIRenderer.h"

#include "./Renderer/DepthStencilView.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/RenderTarget.h"
#include "./Renderer/Shader.h"
#include "./Renderer/AmbientOcclusion.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/VertexDecleration.h"

#include "DefferedPasses.h"

#include "./Utilities/ImGuiSelf.h"
#include "./Utilities/Log.h"




SSAOPass::SSAOPass()
	:mpAOPreparation(std::make_unique<AmbientOcclusion>()),
	mpBlurPassTargets(std::make_unique<RenderTarget>())
{
}

void SSAOPass::Initialize(D3D::DevicePtr& p_device)
{
	mpScreen = std::make_unique<Sprite>(p_device);

	if (mpAOPreparation->Initialize(p_device))
	{
		Log::Info("Initialized Ambient Occlusion.");
	}

	AddVertexAndPixelShader(p_device, ShaderID::ESSAO, L"SSAO.hlsl", L"SSAO.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);
	AddVertexAndPixelShader(p_device, ShaderID::EBlur, L"Screen.hlsl", L"Screen.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);

	for (auto i = 0u; i < GBUFFER_FOR_AO_MAX; ++i)
	{
		GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::ESecondResult + i);
	}
	for (auto i = 0u; i < GBUFFER_FOR_BLUR_MAX; ++i)
	{
		GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EThirdResult + i);
	}

	mpDSV->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT);

	mbIsInitialized = true;

}

void SSAOPass::ExecuteSSAO(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	D3D::DeviceContextPtr& pImmContext = p_graphics->GetImmContextPtr();

	mpAOPreparation->Activate(p_graphics, ENGINE.GetCameraPtr().get());

	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::ESecondResult, GBUFFER_FOR_AO_MAX);

	mpScreen->RenderScreen(pImmContext, mpShaderTable.at(ShaderID::ESSAO).get(), Vector2(0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT), Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));

	mpAOPreparation->Deactivate(p_graphics);

	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::EThirdResult, GBUFFER_FOR_BLUR_MAX);

	GetRenderTargetManager()->Deactivate(pImmContext, RenderTarget::ESecondResult, GBUFFER_FOR_AO_MAX, GBufferID::EResult);

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
				mCurrentScreenNum = i + RenderTarget::ESecondResult;
				mbIsOpen2ndScreen = true;
			}
			ImGui::TreePop();
		}
	}
	//for (auto i = 0u; i < mpBlurPassTargets->GetSize(); ++i)
	//{
	//	if (ImGui::TreeNode(DefferedPass::GBUFFER_NAME[i + GBufferID::ESecondaryResult]))
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
	//	if (ImGui::TreeNode(DefferedPass::GBUFFER_NAME[i + GBufferID::EResult]))
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
}

void SSAOPass::RenderUIForAnotherScreen()
{
	//if (mCurrentScreenNum < GBufferID::EResult) return;


	//D3D::SRVPtr srv = nullptr;
	//if (mCurrentScreenNum < GBufferID::ESecondaryResult) srv = GetRenderTargetPtr()->GetShaderResource(mCurrentScreenNum - GBufferID::EResult);
	//if (mCurrentScreenNum >= GBufferID::ESecondaryResult) srv = GetBlurPassTargetPtr()->GetShaderResource(mCurrentScreenNum - GBufferID::ESecondaryResult);

	//if (mbIsOpen2ndScreen)
	//{
	//	ENGINE.GetUIRenderer()->SetNextWindowSettings(Vector2(0, SCREEN_HEIGHT - 540), Vector2(960, 540));
	//	ImGui::Begin("Screen 2", &mbIsOpen2ndScreen);
	//	ImGui::Image((void*)srv.Get(), ImVec2(960, 540));

	//	ENGINE.GetUIRenderer()->FinishRenderingWindow();
	//}
}
