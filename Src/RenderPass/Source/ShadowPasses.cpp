#include "ShadowPasses.h"

#include "./Engine/GameSystem.h"
//#include "./Engine/MeshRenderer.h"
#include "./Engine/UIRenderer.h"

#include "./Renderer/Blender.h"
#include "./Renderer/DepthStencilView.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Mesh.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/Shader.h"
#include "./Renderer/ShadowMap.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Texture.h"
#include "./Renderer/RenderTarget.h"
#include "./Renderer/VertexDecleration.h"


void ShadowPass::Initialize(D3D::DevicePtr& device)
{
	AddVertexAndPixelShader(device, ShaderType::EToShadow,
        L"ShadowMap.hlsl",
        L"ShadowMap.hlsl",
        "VSshadow", "PSshadow", VEDType::VED_DEFAULT);
    AddVertexAndPixelShader(device, ShaderType::EToShadowForSkinning,
        L"ShadowMap.hlsl",
        L"ShadowMap.hlsl",
        "VSshadowSkinning", "PSshadow", VEDType::VED_SKINNED_MESH);

    mpShadowMap = std::make_unique<ShadowMap>(device);

    mpRenderTargets->Create(device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);

    mpDSV->Initialize(device);
}


void ShadowPass::RenderShadow(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
    D3D::DeviceContextPtr pImmContext = p_graphics->GetImmContextPtr();

    mpShadowMap->Activate(p_graphics, ENGINE.GetLightPtr().get(), ENGINE.GetCameraPtr().get());

    mpRenderTargets->Activate(pImmContext, mpDSV);

    ENGINE.GetMeshRenderer()->RenderMesh(pImmContext, elapsed_time, RenderPassID::EShadowPass);

    p_graphics->ActivateBackBuffer();

    mpRenderTargets->Deactivate(pImmContext, 5);

    mpShadowMap->Deactivate(p_graphics);

    mpShadowMap->RenderUI();
    //ENGINE.GetUIRenderer()->FinishRenderingWindow();


}

void ShadowPass::RenderUI()
{
    ImGui::Image((void*)mpRenderTargets->GetShaderResource(0).Get(), ImVec2(320, 180)/*, ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT)*/);
}
