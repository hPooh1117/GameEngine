#include "DeferredPasses.h"

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
#include "./Utilities/Log.h"






const char* DeferredPass::GBUFFER_NAME[GBufferID::ENUM_GBUFFER_ID_MAX] =
{
	"Color",
	"Normal",
	"Position",
	"CastShadow",
	"Depth",

	"Diffuse",
	"Specular",
	"Skybox",
	"PreLighting",

	"Result",
	"SSAO",

	"SecondaryResult",
	"BluredAO"
};

DeferredPass::DeferredPass()
	:RenderPass(),
	mpPreLightingTarget(std::make_unique<RenderTarget>())
{
}

void DeferredPass::Initialize(D3D::DevicePtr& p_device)
{

	InitializeGBuffer(p_device);
	if (mbIsInitialized) return;

	mpScreen = std::make_unique<Sprite>(p_device);

	InitializeCommonShader(p_device);
	AddVertexAndPixelShader(p_device, ShaderID::EDefferedSkybox, L"EquirectangularToCube.hlsl", L"EquirectangularToCube.hlsl", "VSmainDeffered", "PSmainDeffered", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(p_device, ShaderID::EDefferedSkyboxRevised, L"DefferedSkybox.hlsl", L"DefferedSkybox.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

	//AddVertexAndPixelShader(p_device, ShaderID::EDefferedPhong, L"ToGBufferSimple.hlsl", L"ToGBufferSimple.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	//AddVertexAndPixelShader(p_device, ShaderID::EDefferedPhongForSkinned, L"ToGBufferSimple.hlsl", L"ToGBufferSimple.hlsl", "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);
	//AddVertexAndPixelShader(p_device, ShaderID::EDefferedPhongForSkinning, L"ToGBufferSimple.hlsl", L"ToGBufferSimple.hlsl", "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);

	AddVertexAndPixelShader(p_device, ShaderID::EDefferedShadow, L"ToGBufferShadow.hlsl", L"ToGBufferShadow.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(p_device, ShaderID::EDefferedShadowForSkinned, L"ToGBufferShadow.hlsl", L"ToGBufferShadow.hlsl", "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);
	AddVertexAndPixelShader(p_device, ShaderID::EDefferedShadowForSkinning, L"ToGBufferShadow.hlsl", L"ToGBufferShadow.hlsl", "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);

	AddVertexAndPixelShader(p_device, ShaderID::EDefferedNormal, L"ToGBufferNormal.hlsl", L"ToGBufferNormal.hlsl", "VSmain", "PSmain", VEDType::VED_GEOMETRIC_PRIMITIVE);
	AddVertexAndPixelShader(p_device, ShaderID::EDefferedNormalForSkinned, L"ToGBufferNormal.hlsl", L"ToGBufferNormal.hlsl", "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);
	AddVertexAndPixelShader(p_device, ShaderID::EDefferedNormalForSkinning, L"ToGBufferNormal.hlsl", L"ToGBufferNormal.hlsl", "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);

	AddVertexAndPixelShader(p_device, ShaderID::EDefferedPreLighting, L"DefferedPointLight.hlsl", L"DefferedPointLight.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);

	AddVertexAndPixelShader(p_device, ShaderID::EDefferedSea, L"ToGBufferSea.hlsl", L"ToGBufferSea.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(p_device, ShaderID::EDefferedLighting, L"DefferedLighting.hlsl", L"DefferedLighting.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);


	mbIsInitialized = true;
}

void DeferredPass::InitializeGBuffer(D3D::DevicePtr& p_device)
{
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EColor); // color
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::ENormal);  // normal
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EPosition);  // position
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EShadowMap);  // shadow 
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT, RenderTarget::EDepth);  // depth
	
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EDiffuseLight);
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::ESpecularLight);
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::ESkybox);
	GetRenderTargetManager()->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EFirstResult);


	Log::Info("[DEFERRED PASS] Created GBuffers");

	mpDSV->Create(p_device, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void DeferredPass::RenderDefferedLighting(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	D3D::DeviceContextPtr& pImmContext = p_graphics->GetImmContextPtr();

	p_graphics->SetDepthStencil(GraphicsEngine::DS_TRUE);

	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::EColor, GEOMETRY_BUFFER_SIZE);

	ENGINE.GetMeshRenderer()->Render(p_graphics, elapsed_time, RenderPassID::EDeferredPass);


	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::EDiffuseLight, LIGHT_BUFFER_SIZE);

	GetRenderTargetManager()->Deactivate(pImmContext, RenderTarget::EColor, GEOMETRY_BUFFER_SIZE);


	mpScreen->RenderScreen(pImmContext, mpShaderTable.at(ShaderID::EDefferedLighting).get(), Vector2(0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT), Vector2(SCREEN_WIDTH, SCREEN_HEIGHT));


	p_graphics->ActivateBackBuffer();


	GetRenderTargetManager()->Deactivate(pImmContext, RenderTarget::EDiffuseLight, LIGHT_BUFFER_SIZE, GBufferID::EDiffuse);

}

void DeferredPass::RenderUI(bool b_open)
{
	for (UINT i = 0; i < (GEOMETRY_BUFFER_SIZE + LIGHT_BUFFER_SIZE); ++i)
	{
		if (!b_open) ImGui::SetNextItemOpen(b_open);
		if (ImGui::TreeNode(RenderTarget::RENDER_TARGET_NAME_TABLE[i + RenderTarget::EColor]))
		{
			if (ImGui::ImageButton((void*)GetRenderTargetManager()->GetShaderResource(i + RenderTarget::EColor).Get(), ImVec2(320, 180)))
			{
				//mCurrentScreenNum = i + RenderTarget::EColor;
				//mbIsOpen2ndScreen = true;

				mChosenRenderTarget = RenderTarget::EColor + i;
				mbShowsResult = false;

			}
			ImGui::TreePop();
		}
	}
	//if (ImGui::TreeNode("DepthView"))
	//{
	//	if (ImGui::ImageButton((void*)mpDSV->GetSRVPtr().Get(), ImVec2(320, 180)))
	//	{

	//	}
	//	ImGui::TreePop();
	//}


	//for (auto i = 0; i < mpPreLightingTarget->GetSize(); ++i)
	//{
	//	if (ImGui::TreeNode(GBUFFER_NAME[EPreLighting + i]))
	//	{
	//		if (ImGui::ImageButton((void*)mpPreLightingTarget->GetShaderResource(i).Get(), ImVec2(320, 180)))
	//		{
	//			mCurrentScreenNum = i + GBufferID::EPreLighting;
	//			mbIsOpen2ndScreen = true;
	//		}
	//		ImGui::TreePop();
	//	}
	//}

}

void DeferredPass::RenderUIForAnotherScreen()
{
	//if (mCurrentScreenNum < 0) return;
	//if (mCurrentScreenNum >= GBufferID::EResult) return;

	//D3D::SRVPtr srv = nullptr;
	//if (mCurrentScreenNum < GBufferID::EPreLighting) srv = mpRenderTargets->GetShaderResource(mCurrentScreenNum);
	//if (mCurrentScreenNum >= GBufferID::EPreLighting) srv = mpPreLightingTarget->GetShaderResource(mCurrentScreenNum - GBufferID::EPreLighting);

	//if (mbIsOpen2ndScreen)
	//{
	//	ENGINE.GetUIRenderer()->SetNextWindowSettings(Vector2(0, SCREEN_HEIGHT - 540), Vector2(960, 540));
	//	ImGui::Begin("Screen 2", &mbIsOpen2ndScreen);
	//	ImGui::Image((void*)srv.Get(), ImVec2(960, 540));

	//	ENGINE.GetUIRenderer()->FinishRenderingWindow();
	//}

}

