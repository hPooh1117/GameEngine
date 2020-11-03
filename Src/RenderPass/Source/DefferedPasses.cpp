#include "DefferedPasses.h"

#include "./Engine/GameSystem.h"
#include "./Engine/MeshRenderer.h"
#include "./Engine/UIRenderer.h"

#include "./Renderer/DepthStencilView.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/RenderTarget.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/VertexDecleration.h"

#include "./Utilities/ImGuiSelf.h"


const char* DefferedPass::GBUFFER_NAME[GBufferID::ENUM_GBUFFER_ID_MAX] =
{
	"Color",
	"Normal",
	"Position",
	"CastShadow",
	"Depth",

	"PreLighting",
	"Diffuse",
	"Specular",
	"Skybox",

	"Result",
	"SSAO",

	"SecondaryResult",
	"BluredAO"
};

DefferedPass::DefferedPass()
	:RenderPass(),
	mpPreLightingTarget(std::make_unique<RenderTarget>())
{
}

void DefferedPass::Initialize(D3D::DevicePtr& p_device)
{
	mpScreen = std::make_unique<Sprite>(p_device);

	InitializeCommonShader(p_device);
	AddVertexAndPixelShader(p_device, ShaderType::EDefferedSkybox, L"EquirectangularToCube.hlsl", L"EquirectangularToCube.hlsl", "VSmainDeffered", "PSmainDeffered", VEDType::VED_DEFAULT);

	AddVertexAndPixelShader(p_device, ShaderType::EDefferedPhong, L"ToGBufferSimple.hlsl", L"ToGBufferSimple.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(p_device, ShaderType::EDefferedPhongForSkinned, L"ToGBufferSimple.hlsl", L"ToGBufferSimple.hlsl", "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);
	AddVertexAndPixelShader(p_device, ShaderType::EDefferedPhongForSkinning, L"ToGBufferSimple.hlsl", L"ToGBufferSimple.hlsl", "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);

	AddVertexAndPixelShader(p_device, ShaderType::EDefferedShadow, L"ToGBufferShadow.hlsl", L"ToGBufferShadow.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(p_device, ShaderType::EDefferedShadowForSkinned, L"ToGBufferShadow.hlsl", L"ToGBufferShadow.hlsl", "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);
	AddVertexAndPixelShader(p_device, ShaderType::EDefferedShadowForSkinning, L"ToGBufferShadow.hlsl", L"ToGBufferShadow.hlsl", "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);

	AddVertexAndPixelShader(p_device, ShaderType::EDefferedNormal, L"ToGBufferNormal.hlsl", L"ToGBufferNormal.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(p_device, ShaderType::EDefferedNormalForSkinned, L"ToGBufferNormal.hlsl", L"ToGBufferNormal.hlsl", "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);
	AddVertexAndPixelShader(p_device, ShaderType::EDefferedNormalForSkinning, L"ToGBufferNormal.hlsl", L"ToGBufferNormal.hlsl", "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);

	AddVertexAndPixelShader(p_device, ShaderType::EDefferedPreLighting, L"DefferedPointLight.hlsl", L"DefferedPointLight.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);

	InitializeGBuffer(p_device);
}

void DefferedPass::InitializeGBuffer(D3D::DevicePtr& p_device)
{
	mpRenderTargets->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	mpRenderTargets->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpRenderTargets->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpRenderTargets->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpRenderTargets->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	mpPreLightingTarget->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpPreLightingTarget->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpPreLightingTarget->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpPreLightingTarget->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

	mpDSV->Initialize(p_device);
}

void DefferedPass::RenderDefferedLighting(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	D3D::DeviceContextPtr& pImmContext = p_graphics->GetImmContextPtr();

	mpRenderTargets->Activate(pImmContext, mpDSV);

	ENGINE.GetMeshRenderer()->RenderMesh(pImmContext, elapsed_time, RenderPassID::EDefferedPass);

	ENGINE.GetMeshRenderer()->RenderSkybox(pImmContext, elapsed_time, RenderPassID::EDefferedPass);

	ImGui::Text("DefferedLighting");


	mpPreLightingTarget->Activate(pImmContext, mpDSV);

	mpRenderTargets->Deactivate(pImmContext);

	//p_graphics->SetDepthStencil(GraphicsEngine::DS_TRUE);


	mpScreen->RenderScreen(pImmContext, mpShaderTable.at(ShaderType::EDefferedPreLighting).get(), Vector2(0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT), Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));

	p_graphics->SetDepthStencil(GraphicsEngine::DS_TRUE);

	p_graphics->ActivateBackBuffer();


	if (ENGINE.GetSSAOActivate()) mpPreLightingTarget->Deactivate(pImmContext, GBufferID::EPreLighting);
	else                          mpPreLightingTarget->Deactivate(pImmContext);

}

void DefferedPass::RenderUI()
{
	for (auto i = 0u; i < mpRenderTargets->GetSize(); ++i)
	{
		ImGui::Text(GBUFFER_NAME[i]);
		ImGui::Image((void*)mpRenderTargets->GetShaderResource(i).Get(), ImVec2(320, 180));
	}

	for (auto i = 0; i < mpPreLightingTarget->GetSize(); ++i)
	{
		ImGui::Text(GBUFFER_NAME[EPreLighting + i]);
		ImGui::Image((void*)mpPreLightingTarget->GetShaderResource(i).Get(), ImVec2(320, 180));
	}
}
