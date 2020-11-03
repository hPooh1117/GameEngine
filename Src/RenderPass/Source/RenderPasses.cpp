#include "RenderPasses.h"

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



const std::string RenderPass::SHADER_NAME_TABLE[ShaderType::ENUM_SHADER_MAX] = {
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



//--------------------------------------------------------------------------------------------------------------------------------

const std::wstring RenderPass::SHADER_FILE_PATH = L"./Src/Shaders/";

//--------------------------------------------------------------------------------------------------------------------------------

RenderPass::RenderPass()
	:mpRenderTargets(std::make_unique<RenderTarget>()),
	mpDSV(std::make_unique<DepthStencilView>())
{
}

//--------------------------------------------------------------------------------------------------------------------------------

void RenderPass::InitializeCommonShader(D3D::DevicePtr& device)
{
	AddVertexAndPixelShader(device, ShaderType::ESprite, L"FromGBuffer.hlsl", L"FromGBuffer.hlsl", "VSmain", "PSmain", VEDType::VED_SPRITE);
	//AddVertexAndPixelShader(device, ShaderType::ESkybox, L"SkyBox.hlsl", L"SkyBox.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
	AddVertexAndPixelShader(device, ShaderType::ESkybox, L"EquirectangularToCube.hlsl", L"EquirectangularToCube.hlsl", "VSmain", "PSmain", VEDType::VED_DEFAULT);
}

//--------------------------------------------------------------------------------------------------------------------------------

void RenderPass::AddVertexAndPixelShader(
	D3D::DevicePtr& device,
	ShaderType        id,
	const wchar_t* vs,
	const wchar_t* ps,
	const char* vs_entry,
	const char* ps_entry,
	const VEDType& vedType)
{
	if (mpShaderTable.find(id) != mpShaderTable.end())
	{
		// テーブル上にすでに存在していた場合はスルー
		Log::Warning("[RENDER PASS] The Shader already exists. (%s)", SHADER_NAME_TABLE[id].c_str());
		return;
	}
	std::wstring vsFile = SHADER_FILE_PATH + std::wstring(vs);
	std::wstring psFile = SHADER_FILE_PATH + std::wstring(ps);


	mpShaderTable[id] = std::make_unique<Shader>();
	mpShaderTable[id]->createShader(device, vsFile, psFile, vs_entry, ps_entry, vedType);
}

//--------------------------------------------------------------------------------------------------------------------------------

void RenderPass::AddGeometryShader(
	D3D::DevicePtr& device,
	ShaderType       id,
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

	mpShaderTable[id]->createGeometryShader(device, gsFile, gs_entry);
}

//--------------------------------------------------------------------------------------------------------------------------------

void RenderPass::AddDomainAndHullShader(
	D3D::DevicePtr& device, 
	ShaderType       id,
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

	mpShaderTable[id]->createHullAndDomain(device, hsFile, hs_entry, ds_entry);
}

//--------------------------------------------------------------------------------------------------------------------------------

const std::unique_ptr<Shader>& RenderPass::GetShaderPtr(ShaderType id)
{
	auto it = mpShaderTable.find(id);
	if (it != mpShaderTable.end()) return it->second;
	return std::unique_ptr<Shader>();
}


//--------------------------------------------------------------------------------------------------------------------------------

void RenderPass::SetShader(D3D::DeviceContextPtr& p_imm_context, ShaderType id)
{
	mpShaderTable.at(id)->activateShaders(p_imm_context);
}

//void RenderPass::SetBackBuffer(std::unique_ptr<GraphicsEngine>& p_graphics)
//{
//	p_graphics->ActivateBackBuffer();
//}

//--------------------------------------------------------------------------------------------------------------------------------
