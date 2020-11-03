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

	for (auto i = 0u; i < GBUFFER_FOR_AO_MAX; ++i)
	{
		mpRenderTargets->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	}
	for (auto i = 0u; i < GBUFFER_FOR_BLUR_MAX; ++i)
	{
		mpBlurPassTargets->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	}

	AddVertexAndPixelShader(p_device, ShaderType::ESSAO, L"SSAO.hlsl", L"SSAO.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);
	AddVertexAndPixelShader(p_device, ShaderType::EBlur, L"Screen.hlsl", L"Screen.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);

	mpDSV->Initialize(p_device);
}

void SSAOPass::ExecuteSSAO(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	D3D::DeviceContextPtr& pImmContext = p_graphics->GetImmContextPtr();

	mpAOPreparation->Activate(p_graphics, ENGINE.GetCameraPtr().get());

	mpRenderTargets->Activate(pImmContext, mpDSV);

	mpScreen->RenderScreen(pImmContext, mpShaderTable.at(ShaderType::ESSAO).get(), Vector2(0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT), Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));

	mpAOPreparation->Deactivate(p_graphics);

	mpBlurPassTargets->Activate(pImmContext, mpDSV);

	mpRenderTargets->Deactivate(pImmContext, GBufferID::EResult);

	mpScreen->RenderScreen(pImmContext, mpShaderTable.at(ShaderType::EBlur).get(), Vector2(0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT), Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));

	p_graphics->SetDepthStencil(GraphicsEngine::DS_TRUE);

	p_graphics->ActivateBackBuffer();

	mpBlurPassTargets->Deactivate(pImmContext);

}

void SSAOPass::RenderUI()
{
	for (auto i = 0u; i < mpRenderTargets->GetSize(); ++i)
	{
		ImGui::Text(DefferedPass::GBUFFER_NAME[i + GBufferID::EResult]);
		ImGui::Image((void*)mpRenderTargets->GetShaderResource(i).Get(), ImVec2(320, 180));
	}
	for (auto i = 0u; i < mpBlurPassTargets->GetSize(); ++i)
	{
		ImGui::Text(DefferedPass::GBUFFER_NAME[i + GBufferID::ESecondaryResult]);
		ImGui::Image((void*)mpBlurPassTargets->GetShaderResource(i).Get(), ImVec2(320, 180));
	}
}

void SSAOPass::RenderUIForRuntime()
{
	mpAOPreparation->RenderUI();
}
