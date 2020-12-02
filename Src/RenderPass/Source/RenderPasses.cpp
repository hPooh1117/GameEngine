#include "RenderPasses.h"

#include "./Engine/GameSystem.h"
#include "./Engine/UIRenderer.h"
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
#include "./Utilities/ImGuiSelf.h"


const std::string RenderPass::SHADER_NAME_TABLE[ShaderID::ENUM_SHADER_MAX] = {
	"Sprite",
	"Lambert",
	"Phong",
	"Line",
	"Skybox",

	"EnvironmentMap",
	"NormalMap",
	"ToonGeo",
	"ForwardBRDF",

};

const std::wstring RenderPass::SHADER_FILE_PATH = L"./Src/Shaders/";


std::unique_ptr<RenderTarget>  RenderPass::mpRenderTargetManager = std::make_unique<RenderTarget>();
int RenderPass::mCurrentScreenNum = -1;
bool RenderPass::mbIsOpen2ndScreen = false;
//--------------------------------------------------------------------------------------------------------------------------------

RenderPass::RenderPass()
	:
	mpDSV(std::make_unique<DepthStencilView>()),
	mbIsInitialized(false),
	mbWannaReloadShader(false),
	mChosenShaderID(0)
{
}

//--------------------------------------------------------------------------------------------------------------------------------

void RenderPass::InitializeCommonShader(D3D::DevicePtr& device)
{
	AddVertexAndPixelShader(device, ShaderID::ESprite, L"FromGBuffer.hlsl", L"FromGBuffer.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);
	//AddVertexAndPixelShader(device, ShaderID::ESkybox, L"SkyBox.hlsl", L"SkyBox.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderID::ESkybox, L"EquirectangularToCube.hlsl", L"EquirectangularToCube.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
}

//--------------------------------------------------------------------------------------------------------------------------------

void RenderPass::AddVertexAndPixelShader(
	D3D::DevicePtr& device,
	UINT        id,
	const wchar_t* vs,
	const wchar_t* ps,
	const char* vs_entry,
	const char* ps_entry,
	const VEDType& vedType)
{
	if (mpShaderTable.find(id) != mpShaderTable.end())
	{
		// テーブル上にすでに存在していた場合はスルー
		Log::Warning("[RENDER PASS] The Shader already exists. ");
		return;
	}
	std::wstring vsFile = SHADER_FILE_PATH + std::wstring(vs);
	std::wstring psFile = SHADER_FILE_PATH + std::wstring(ps);


	mpShaderTable[id] = std::make_unique<Shader>();
	//mpShaderTable[id]->createShader(device, vsFile, psFile, vs_entry, ps_entry, vedType);
	mpShaderTable[id]->CreateShader(device, ShaderType::EVertexShader, vsFile, vs_entry, vedType);
	mpShaderTable[id]->CreateShader(device, ShaderType::EPixelShader, psFile, ps_entry);

	mpShaderNameTable.emplace(id, vs);
}

//--------------------------------------------------------------------------------------------------------------------------------

void RenderPass::AddGeometryShader(
	D3D::DevicePtr& device,
	UINT           id,
	const wchar_t* gs,
	const char* gs_entry)
{
	if (mpShaderTable.find(id) == mpShaderTable.end())
	{
		// テーブル上に同じ名前の頂点ピクセルシェーダがない場合、スルー
		Log::Warning("[RENDER PASS] Vertex Shader and Pixel Shader doesn't exist. (%s)", SHADER_NAME_TABLE[id].c_str());
		return;
	}

	std::wstring gsFile = SHADER_FILE_PATH + std::wstring(gs);

	//mpShaderTable[id]->CreateGeometryShader(device, gsFile, gs_entry);
	mpShaderTable[id]->CreateShader(device, ShaderType::EGeometryShader, gsFile, gs_entry);

}

//--------------------------------------------------------------------------------------------------------------------------------

void RenderPass::AddDomainAndHullShader(
	D3D::DevicePtr& device,
	UINT           id,
	const wchar_t* ds,
	const wchar_t* hs,
	const char* ds_entry,
	const char* hs_entry)
{
	if (mpShaderTable.find(id) == mpShaderTable.end())
	{
		// テーブル上に同じ名前の頂点ピクセルシェーダがない場合、スルー
		Log::Warning("[RENDER PASS] Vertex Shader and Pixel Shader doesn't exist. (%s)", SHADER_NAME_TABLE[id].c_str());
		return;
	}


	std::wstring hsFile = SHADER_FILE_PATH + std::wstring(hs);

	//mpShaderTable[id]->CreateHullAndDomain(device, hsFile, hs_entry, ds_entry);
	mpShaderTable[id]->CreateShader(device, ShaderType::EHullShader, hsFile, hs_entry);
	mpShaderTable[id]->CreateShader(device, ShaderType::EDomainShader, hsFile, ds_entry);


}

//void RenderPass::CreateRenderTarget(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT rt_id)
//{
//	mpRenderTargetTable.emplace(rt_id, std::make_unique<RenderTarget>());
//	mpRenderTargetTable[rt_id]->Create(p_device, width, height, format, rt_id);
//}
//
//void RenderPass::CreateCubeRenderTarget(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT mip_slice, UINT rt_id)
//{
//	mpRenderTargetTable.emplace(rt_id, std::make_unique<RenderTarget>());
//	mpRenderTargetTable[rt_id]->CreateCube(p_device, width, height, format, mip_slice, rt_id);
//}

//void RenderPass::ActivateRenderTargets(D3D::DeviceContextPtr& p_imm_context, UINT num_activate, UINT min_rt_ID)
//{
//	auto it = mpRenderTargetTable.find(min_rt_ID);
//
//	while (it != mpRenderTargetTable.end())
//	{
//		it->second->Activate(p_imm_context, mpDSV);
//		/*it->second->Activate(p_imm_context, mpDSV);*/
//		++it;
//		if (--num_activate <= 0) return;
//	}
//}
//
//void RenderPass::SetRenderTargetsForSRV(D3D::DeviceContextPtr& p_imm_context, UINT num_activate, UINT min_rt_ID, UINT slot)
//{
//	int slotNo = slot;
//	auto it = mpRenderTargetTable.find(min_rt_ID);
//	while (it != mpRenderTargetTable.end())
//	{
//		it->second->Deactivate(p_imm_context, slotNo);
//		++it;
//		++slotNo;
//		if (--num_activate <= 0) return;
//	}
//}

void RenderPass::CheckActivatedShaders()
{
	bool isActivated = false;
	for (auto& shader : mpShaderTable)
	{
		isActivated = shader.second->GetIsActivated();
		Log::Info("ShaderID : %d -> %s", shader.first, isActivated > 0 ? "Activated" : "Useless");
	}
}

void RenderPass::ShowShaderList(std::unique_ptr<GraphicsEngine>& p_graphics, const char* current_pass)
{
	if (ImGui::BeginMenu(current_pass))
	{
		for (auto& shaderName : mpShaderNameTable)
		{
			char multibyte[256];
			WideCharToMultiByte(CP_ACP, 0, shaderName.second.c_str(), -1, multibyte, 256, NULL, NULL);

			if (ImGui::MenuItem(multibyte))
			{
				//ImGui::OpenPopup("Reload Shader?");
				mbWannaReloadShader = true;
				mChosenShaderID = shaderName.first;
			}
		}
		ImGui::EndMenu();
	}

}

void RenderPass::ReloadShaderFile(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	if (mbWannaReloadShader)
	{
		if (mpShaderTable.at(mChosenShaderID)->ReloadShader(p_graphics->GetDevicePtr(), SHADER_FILE_PATH + mpShaderNameTable.at(mChosenShaderID)))
			mbWannaReloadShader = false;
	}
}

void RenderPass::Clear()
{
	mpRenderTargetManager->ClearAll();
	mCurrentScreenNum = 0;
	mbIsOpen2ndScreen = false;
}


//--------------------------------------------------------------------------------------------------------------------------------

const std::unique_ptr<Shader>& RenderPass::GetShaderPtr(UINT shader_id)
{
	auto it = mpShaderTable.find(shader_id);
	if (it != mpShaderTable.end()) return it->second;
	return std::unique_ptr<Shader>();
}


//--------------------------------------------------------------------------------------------------------------------------------

void RenderPass::SetShader(D3D::DeviceContextPtr& p_imm_context, ShaderID id)
{
	mpShaderTable.at(id)->ActivateShaders(p_imm_context);
}

//void RenderPass::SetBackBuffer(std::unique_ptr<GraphicsEngine>& p_graphics)
//{
//	p_graphics->ActivateBackBuffer();
//}

//--------------------------------------------------------------------------------------------------------------------------------
