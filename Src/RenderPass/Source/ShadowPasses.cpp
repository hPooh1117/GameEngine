#include "ShadowPasses.h"

#include "./Engine/GameSystem.h"
//#include "./Engine/MeshRenderer.h"
#include "./Engine/UIRenderer.h"

#include "./Renderer/Blender.h"
#include "./Renderer/DepthStencilView.h"
#include "./Renderer/Mesh.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/Shader.h"
//#include "./Renderer/ShadowMap.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Texture.h"
#include "./Renderer/Renderer.h"
#include "./Renderer/RenderTarget.h"
#include "./Renderer/VertexDecleration.h"

#include "./Utilities/Log.h"



void ShadowPass::Initialize(Graphics::GraphicsDevice* device)
{
	auto pDevice = device->GetDevicePtr();
	GetRenderTargetManager()->Create(pDevice, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT, RenderTarget::EShadow);
	
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

	mpDSV->Create(pDevice, SCREEN_WIDTH, SCREEN_HEIGHT);

	mbIsInitialized = true;
}


void ShadowPass::RenderShadow(Graphics::GraphicsDevice* device, float elapsed_time)
{

	D3D::DeviceContextPtr pImmContext = device->GetImmContextPtr();

	//p_graphics->OMSetDepthStencilState(GraphicsDevice::DS_TRUE);

	mpShadowMap->Activate(device, ENGINE.GetLightPtr().get(), ENGINE.GetCameraPtr().get());

	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::EShadow);

	ENGINE.GetRenderer()->GetMeshRenderer()->RenderMesh(device, elapsed_time, RenderPassID::EShadowPass);

	device->ActivateBackBuffer();

	GetRenderTargetManager()->Deactivate(pImmContext, RenderTarget::EShadow, 1, 5);

	mpShadowMap->Deactivate(device);

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
			//mCurrentScreenNum = RenderTarget::EShadow;
			//mbIsOpen2ndScreen = true;

			mChosenRenderTarget = RenderTarget::EShadow;
			mbShowsResult = false;

		}
		ImGui::TreePop();
	}
}
