#include "ForwardPasses.h"

#include "./Engine/GameSystem.h"
#include "./Engine/UIRenderer.h"


#include "./Renderer/Blender.h"
#include "./Renderer/DepthStencilView.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Mesh.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/RenderTarget.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Texture.h"
#include "./Renderer/VertexDecleration.h"


//--------------------------------------------------------------------------------------------------------------------------------

ForwardPass::ForwardPass()
	:RenderPass()
{

}

//--------------------------------------------------------------------------------------------------------------------------------

void ForwardPass::Initialize(D3D::DevicePtr& device)
{
	InitializeCommonShader(device);

	AddVertexAndPixelShader(device, ShaderType::ELambert, L"Lambert.hlsl", L"Lambert.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

	AddVertexAndPixelShader(device, ShaderType::ELambertSkinned, L"Lambert.hlsl", L"Lambert.hlsl", "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);

	AddVertexAndPixelShader(device, ShaderType::EPhong, L"Phong.hlsl", L"Phong.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

	AddVertexAndPixelShader(device, ShaderType::ELine, L"line.hlsl", L"line.hlsl", "VSMainDuplicate", "PSmain", VEDType::VED_GEOMETRIC_PRIMITIVE);
	AddGeometryShader(device,       ShaderType::ELine, L"line.hlsl", "GSmainDuplicate");

	AddVertexAndPixelShader(device, ShaderType::EEnvironmentMap, L"EnvironmentMap.hlsl", L"EnvironmentMap.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);


	AddVertexAndPixelShader(device, ShaderType::ENormalMap, L"NormalMap.hlsl", L"NormalMap.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

	AddVertexAndPixelShader(device, ShaderType::EToonGeo, L"ToonGeo.hlsl", L"ToonGeo.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddGeometryShader(device,       ShaderType::EToonGeo, L"ToonGeo.hlsl", "GSmain");

	AddVertexAndPixelShader(device, ShaderType::EForwardPBR, L"ForwardBRDF.hlsl", L"ForwardBRDF.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderType::EForwardPBRSetting, L"ForwardBRDF.hlsl", L"ForwardBRDF.hlsl", "VSmain", "PSmainSetting", VEDType::VED_DEFAULT);


	AddVertexAndPixelShader(device, ShaderType::ESpotLightPhong, L"PhongForSpotLight.hlsl", L"PhongForSpotLight.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderType::ESpotLightPhongForSkinning, L"PhongForSpotLight.hlsl", L"PhongForSpotLight.hlsl", "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);
	AddVertexAndPixelShader(device, ShaderType::ESpotLightBump, L"PhongForSpotLight.hlsl", L"PhongForSpotLight.hlsl", "VSmainBump", "PSmainBump", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderType::ESpotLightOcean, L"SpotLightSea.hlsl", L"SpotLightSea.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

	AddVertexAndPixelShader(device, ShaderType::EFromShadow, L"ShadowMap.hlsl", L"ShadowMap.hlsl", "VSmain2", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderType::EFromShadowForSkinning, L"ShadowMap.hlsl", L"ShadowMap.hlsl", "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);

	AddVertexAndPixelShader(device, ShaderType::ETextureTest, L"EquirectangularToCube.hlsl", L"EquirectangularToCube.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);

	mpRenderTargets->Create(device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	mpDSV->Initialize(device);
}

//--------------------------------------------------------------------------------------------------------------------------------

void ForwardPass::RenderForwardLighting(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	D3D::DeviceContextPtr pImmContext = p_graphics->GetImmContextPtr();

	p_graphics->mBlender.SetBlendState(pImmContext, Blender::BLEND_ALPHA);

	mpRenderTargets->Activate(pImmContext, mpDSV);


	ENGINE.GetMeshRenderer()->RenderMesh(pImmContext, elapsed_time, RenderPassID::EForwardPass);


	ENGINE.GetMeshRenderer()->RenderSkybox(pImmContext, elapsed_time, RenderPassID::EForwardPass);


	p_graphics->SetDepthStencil(GraphicsEngine::DS_TRUE);

	p_graphics->ActivateBackBuffer();


	mpRenderTargets->Deactivate(pImmContext);
}

void ForwardPass::RenderUI()
{
	ImGui::Text("Lighting");

	ImGui::Image((void*)mpRenderTargets->GetShaderResource(0).Get(), ImVec2(320, 180));
}

//--------------------------------------------------------------------------------------------------------------------------------
