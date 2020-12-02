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

#include "./Utilities/Log.h"



void ShadowPass::Initialize(D3D::DevicePtr& device)
{
	GetRenderTargetManager()->Create(device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTarget::EShadow);
	
	if (mbIsInitialized) return;

	AddVertexAndPixelShader(device, ShaderID::EToShadow,
		L"ShadowMap.hlsl",
		L"ShadowMap.hlsl",
		"VSshadow", "PSshadow", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderID::EToShadowForSkinning,
		L"ShadowMap.hlsl",
		L"ShadowMap.hlsl",
		"VSshadowSkinning", "PSshadow", VEDType::VED_SKINNED_MESH);

	mpShadowMap = std::make_unique<ShadowMap>(device);

	mpDSV->Create(device, SCREEN_WIDTH, SCREEN_HEIGHT);

	mbIsInitialized = true;
}


void ShadowPass::RenderShadow(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	D3D::DeviceContextPtr pImmContext = p_graphics->GetImmContextPtr();

	//p_graphics->SetDepthStencil(GraphicsEngine::DS_TRUE);

	mpShadowMap->Activate(p_graphics, ENGINE.GetLightPtr().get(), ENGINE.GetCameraPtr().get());

	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::EShadow);

	ENGINE.GetMeshRenderer()->RenderMesh(pImmContext, elapsed_time, RenderPassID::EShadowPass);

	p_graphics->ActivateBackBuffer();

	GetRenderTargetManager()->Deactivate(pImmContext, RenderTarget::EShadow, 1, 5);

	mpShadowMap->Deactivate(p_graphics);

	//ENGINE.GetUIRenderer()->FinishRenderingWindow();


}

void ShadowPass::RenderUI(bool b_open)
{
	if (!b_open) ImGui::SetNextItemOpen(b_open);

	if (ImGui::TreeNode(RenderTarget::RENDER_TARGET_NAME_TABLE[RenderTarget::EShadow]))
	{
		mpShadowMap->RenderUI();

		if (ImGui::ImageButton((void*)GetRenderTargetManager()->GetShaderResource(RenderTarget::EShadow).Get(), ImVec2(320, 180)))
		{
			mCurrentScreenNum = RenderTarget::EShadow;
			mbIsOpen2ndScreen = true;
		}
		ImGui::TreePop();
	}
}
