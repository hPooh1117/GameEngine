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

#include "./Utilities/Log.h"




//--------------------------------------------------------------------------------------------------------------------------------

ForwardPass::ForwardPass()
	:RenderPass()
{

}

//--------------------------------------------------------------------------------------------------------------------------------

void ForwardPass::Initialize(D3D::DevicePtr& device)
{
	InitializeCommonShader(device);

	AddVertexAndPixelShader(device, ShaderID::ELambert,                   L"Lambert.hlsl",                 L"Lambert.hlsl",               "VSmain", "PSmain", VEDType::VED_DEFAULT);
																          				                   					              
	//AddVertexAndPixelShader(device, ShaderID::ELambertSkinned,            L"Lambert.hlsl",                 L"Lambert.hlsl",               "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);
																          				                   					              
	//AddVertexAndPixelShader(device, ShaderID::EPhong,                     L"Phong.hlsl",                   L"Phong.hlsl",                 "VSmain", "PSmain", VEDType::VED_DEFAULT);
																          				                   						          
	AddVertexAndPixelShader(device, ShaderID::ELine,                      L"line.hlsl",                    L"line.hlsl",                  "VSMainDuplicate", "PSmain", VEDType::VED_GEOMETRIC_LINE);
	AddGeometryShader(device,       ShaderID::ELine,                      L"line.hlsl",                    "GSmainDuplicate");	          
																          						          						          
	AddVertexAndPixelShader(device, ShaderID::EEnvironmentMap,            L"EnvironmentMap.hlsl",          L"EnvironmentMap.hlsl",        "VSmain", "PSmain", VEDType::VED_DEFAULT);
																          														          
																          														          
	AddVertexAndPixelShader(device, ShaderID::ENormalMap,                 L"NormalMap.hlsl",               L"NormalMap.hlsl",             "VSmain", "PSmain", VEDType::VED_DEFAULT);
																          					               						          
	AddVertexAndPixelShader(device, ShaderID::EToonGeo,                   L"ToonGeo.hlsl",                 L"ToonGeo.hlsl",               "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddGeometryShader(device,       ShaderID::EToonGeo,                   L"ToonGeo.hlsl",                 "GSmain");
																          					               
	AddVertexAndPixelShader(device, ShaderID::EForwardPBR,                L"ForwardBRDF.hlsl",             L"ForwardBRDF.hlsl",           "VSmain", "PSmain2", VEDType::VED_GEOMETRIC_PRIMITIVE);
	//AddVertexAndPixelShader(device, ShaderID::EForwardPBRForSkinned,      L"ForwardBRDF.hlsl",             L"ForwardBRDF.hlsl",           "VSmainS", "PSmain2", VEDType::VED_SKINNED_MESH);


	AddVertexAndPixelShader(device, ShaderID::ESpotLightPhong,            L"PhongForSpotLight.hlsl",       L"PhongForSpotLight.hlsl",     "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderID::ESpotLightPhongForSkinning, L"PhongForSpotLight.hlsl",       L"PhongForSpotLight.hlsl",     "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);
	AddVertexAndPixelShader(device, ShaderID::ESpotLightBump,             L"PhongForSpotLight.hlsl",       L"PhongForSpotLight.hlsl",     "VSmainBump", "PSmainBump", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderID::ESpotLightOcean,            L"SpotLightSea.hlsl",            L"SpotLightSea.hlsl",          "VSmain", "PSmain", VEDType::VED_DEFAULT);
																	      
	AddVertexAndPixelShader(device, ShaderID::EFromShadow,                L"ShadowMap.hlsl",               L"ShadowMap.hlsl",             "VSmain2", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderID::EFromShadowForSkinning,     L"ShadowMap.hlsl",               L"ShadowMap.hlsl",             "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);
																	      
	//AddVertexAndPixelShader(device, ShaderID::ETextureTest,               L"EquirectangularToCube.hlsl",   L"EquirectangularToCube.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
																	      
	AddVertexAndPixelShader(device, ShaderID::EFlat,                      L"FlatShaderGeo.hlsl",           L"FlatShaderGeo.hlsl",         "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddGeometryShader(device,       ShaderID::EFlat,                      L"FlatShaderGeo.hlsl",           "GSmain");
																	      						           
	//AddVertexAndPixelShader(device, ShaderID::EFurShader,                 L"FurShaderGeo.hlsl",            L"FurShaderGeo.hlsl",          "VSmain", "PSmain", VEDType::VED_DEFAULT);
	//AddGeometryShader(device,       ShaderID::EFurShader,                 L"FurShaderGeo.hlsl",            "GSmain");

	//AddVertexAndPixelShader(device, ShaderID::ECubemapConvolution,        L"CubemapConvolution.hlsl",	   L"CubemapConvolution.hlsl",	  "VSmain", "PSmain", VEDType::VED_DEFAULT);

	AddVertexAndPixelShader(device, ShaderID::EUseCubeMap,				  L"UseCubeMap.hlsl",  L"UseCubeMap.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderID::EReflectSea,				  L"ReflectSea.hlsl",  L"ReflectSea.hlsl", "VSmain", "PSmain", VEDType::VED_GEOMETRIC_PRIMITIVE);

	AddVertexAndPixelShader(device, ShaderID::ECubeMapEnv,					  L"Skybox.hlsl",      L"Skybox.hlsl",     "VSmain", "PSmain", VEDType::VED_DEFAULT);

	GetRenderTargetManager()->Create(device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, RenderTarget::EForward);


	mpDSV->Create(device, SCREEN_WIDTH, SCREEN_HEIGHT);

	mbIsInitialized = true;

}

//--------------------------------------------------------------------------------------------------------------------------------

void ForwardPass::RenderForwardLighting(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	D3D::DeviceContextPtr pImmContext = p_graphics->GetImmContextPtr();

	p_graphics->mBlender.SetBlendState(pImmContext, Blender::BLEND_ALPHA);

	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::EForward, 1);


	ENGINE.GetMeshRenderer()->RenderMesh(pImmContext, elapsed_time, RenderPassID::EForwardPass);
	ENGINE.GetMeshRenderer()->RenderSkybox(pImmContext, elapsed_time, RenderPassID::EForwardPass);


	p_graphics->SetDepthStencil(GraphicsEngine::DS_TRUE);

	p_graphics->ActivateBackBuffer();


	GetRenderTargetManager()->Deactivate(pImmContext, RenderTarget::EForward, 1);
}

//--------------------------------------------------------------------------------------------------------------------------------

void ForwardPass::RenderUI(bool b_open)
{
	if (!b_open) ImGui::SetNextItemOpen(b_open);

	if (ImGui::TreeNode(RenderTarget::RENDER_TARGET_NAME_TABLE[RenderTarget::EForward]))
	{
		if (ImGui::ImageButton((void*)GetRenderTargetManager()->GetShaderResource(RenderTarget::EForward).Get(), ImVec2(320, 180)))
		{
			mCurrentScreenNum = RenderTarget::EForward;
			mbIsOpen2ndScreen = true;
		}

		ImGui::TreePop();

	}
}

//--------------------------------------------------------------------------------------------------------------------------------
