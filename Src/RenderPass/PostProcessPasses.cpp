#include "PostProcessPasses.h"

#include "./Engine/GameSystem.h"

#include "./Renderer/DepthStencilView.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Texture.h"
#include "./Renderer/RenderTarget.h"

PostProcessPass::PostProcessPass()
	:RenderPass()
{
}

void PostProcessPass::Initialize(D3D::DevicePtr& device)
{
	InitializeCommonShader(device);

	mpScreen = std::make_unique<Sprite>(device);

	AddVertexAndPixelShader(device, ShaderType::EPostEffect, L"PostEffect.hlsl", L"PostEffect.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);
}

void PostProcessPass::RenderPostProcess(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	D3D::DeviceContextPtr& pImmContext = p_graphics->GetImmContextPtr();


	mpScreen->RenderScreen(pImmContext, mpShaderTable.at(ShaderType::EPostEffect).get(), Vector2(0.5f * ENGINE.mCurrentWidth, 0.5f * ENGINE.mCurrentHeight), Vector2(ENGINE.mCurrentWidth, ENGINE.mCurrentHeight));
}

