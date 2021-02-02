#include "MakeCubeMapPasses.h"

#include "./Engine/GameSystem.h"
//#include "./Engine/MeshRenderer.h"
#include "./Engine/UIRenderer.h"

#include "./Renderer/Blender.h"
#include "./Renderer/DepthStencilView.h"
#include "./Renderer/Mesh.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/Shader.h"
#include "./Renderer/ShadowMap.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Texture.h"
#include "./Renderer/Renderer.h"
#include "./Renderer/RenderTarget.h"
#include "./Renderer/VertexDecleration.h"
#include "./Renderer/SubView.h"

#include "./Utilities/Log.h"



MakeCubeMapPass::MakeCubeMapPass() :
	mpSubView(std::make_unique<SubView>()),
	mPos({ 0,0,0 }),
	mInterval(0.0f)
{
}

void MakeCubeMapPass::Initialize(Graphics::GraphicsDevice* device)
{
	auto pDevice = device->GetDevicePtr();
	GetRenderTargetManager()->CreateCube(pDevice, CUBE_MAP_WIDTH, CUBE_MAP_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, 1, RenderTarget::ECubemap);
	//CreateCubeRenderTarget(device, CUBE_MAP_WIDTH, CUBE_MAP_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1, RenderTarget::ECubemap);

	if (mbIsInitialized) return;

	mpDSV->CreateCubeDepthStencil(pDevice, CUBE_MAP_WIDTH, CUBE_MAP_HEIGHT);

	D3D11_BUFFER_DESC cbDesc = {};
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(CBufferForView);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;

	pDevice->CreateBuffer(&cbDesc, nullptr, mpCBufferForView.GetAddressOf());


	InitializeCommonShader(device);
	AddVertexAndPixelShader(device, ShaderID::EMakeCubeMap, L"MakeCubeMap.hlsl", L"MakeCubeMap.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddGeometryShader(device, ShaderID::EMakeCubeMap, L"MakeCubeMap.hlsl", "GSmain");
	AddVertexAndPixelShader(device, ShaderID::EMakeCubeMapForSkinning, L"MakeCubeMap.hlsl", L"MakeCubeMap.hlsl", "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);
	AddGeometryShader(device, ShaderID::EMakeCubeMapForSkinning, L"MakeCubeMap.hlsl", "GSmain");


	mbIsInitialized = true;
}

void MakeCubeMapPass::MakeCubeMap(Graphics::GraphicsDevice* device, float elapsed_time)
{
	D3D::DeviceContextPtr pImmContext = device->GetImmContextPtr();

	GetRenderTargetManager()->Activate(pImmContext, mpDSV, RenderTarget::ECubemap, 1);

	device->OMSetDepthStencilState(Graphics::DS_TRUE);

	device->RSSetViewports(CUBE_MAP_WIDTH, CUBE_MAP_HEIGHT);

	// コンスタントバッファでキューブの面それぞれの視点を送る
	CBufferForView cb;
	mpSubView->SetProjection(DirectX::XM_PIDIV2, static_cast<float>(CUBE_MAP_WIDTH) / static_cast<float>(CUBE_MAP_HEIGHT), 0.1f, 500.0f);
	mpSubView->Set(mPos, { mPos.x + mInterval, mPos.y, mPos.z }, { 0,1,0 });
	mpSubView->Activate();
	cb.view[0] = mpSubView->GetView();

	mpSubView->SetProjection(DirectX::XM_PIDIV2, static_cast<float>(CUBE_MAP_WIDTH) / static_cast<float>(CUBE_MAP_HEIGHT), 0.1f, 500.0f);
	mpSubView->Set(mPos, { mPos.x - mInterval, mPos.y, mPos.z }, { 0,1,0 });
	mpSubView->Activate();
	cb.view[1] = mpSubView->GetView();

	mpSubView->SetProjection(DirectX::XM_PIDIV2, static_cast<float>(CUBE_MAP_WIDTH) / static_cast<float>(CUBE_MAP_HEIGHT), 0.1f, 500.0f);
	mpSubView->Set(mPos, { mPos.x, mPos.y + mInterval, mPos.z }, { 0,0,-1 });
	mpSubView->Activate();
	cb.view[2] = mpSubView->GetView();

	mpSubView->SetProjection(DirectX::XM_PIDIV2, static_cast<float>(CUBE_MAP_WIDTH) / static_cast<float>(CUBE_MAP_HEIGHT), 0.1f, 500.0f);
	mpSubView->Set(mPos, { mPos.x, mPos.y - mInterval, mPos.z }, { 0,0,1 });
	mpSubView->Activate();
	cb.view[3] = mpSubView->GetView();

	mpSubView->SetProjection(DirectX::XM_PIDIV2, static_cast<float>(CUBE_MAP_WIDTH) / static_cast<float>(CUBE_MAP_HEIGHT), 0.1f, 500.0f);
	mpSubView->Set(mPos, { mPos.x, mPos.y, mPos.z + mInterval }, { 0,1,0 });
	mpSubView->Activate();
	cb.view[4] = mpSubView->GetView();

	mpSubView->SetProjection(DirectX::XM_PIDIV2, static_cast<float>(CUBE_MAP_WIDTH) / static_cast<float>(CUBE_MAP_HEIGHT), 0.1f, 500.0f);
	mpSubView->Set(mPos, { mPos.x, mPos.y, mPos.z - mInterval }, { 0,1,0 });
	mpSubView->Activate();
	cb.view[5] = mpSubView->GetView();

	cb.proj = mpSubView->GetProj();
	pImmContext->UpdateSubresource(mpCBufferForView.Get(), 0, nullptr, &cb, 0, 0);
	pImmContext->GSSetConstantBuffers(3, 1, mpCBufferForView.GetAddressOf());
	pImmContext->PSSetConstantBuffers(3, 1, mpCBufferForView.GetAddressOf());

	ENGINE.GetRenderer()->GetMeshRenderer()->Render(device, elapsed_time, RenderPassID::ECubeMapPass);


	device->ActivateBackBuffer();

	GetRenderTargetManager()->Deactivate(pImmContext, RenderTarget::ECubemap, 1, 9);
}

void MakeCubeMapPass::RenderUI(bool b_open)
{
	if (!b_open) ImGui::SetNextItemOpen(b_open);

	if (ImGui::TreeNode(RenderTarget::RENDER_TARGET_NAME_TABLE[RenderTarget::ECubemap]))
	{
		//if (ImGui::ImageButton((void*)mpRenderTargets->GetShaderResource(0).Get(), ImVec2(320, 180))) Log::Info("shader");
		if (ImGui::ImageButton((void*)GetRenderTargetManager()->GetShaderResource(RenderTarget::ECubemap).Get(), ImVec2(320, 180)))
		{
			//mbIsOpen2ndScreen = true;
			//mCurrentScreenNum = RenderTarget::ECubemap;

			mChosenRenderTarget = RenderTarget::ECubemap;
			mbShowsResult = false;

		}
		ImGui::TreePop();
	}
}

void MakeCubeMapPass::SetOriginPoint(const Vector3& pos, float interval)
{
	mPos = pos;
	mInterval = interval;
}
