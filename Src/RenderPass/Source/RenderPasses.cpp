#include "RenderPasses.h"

#include "./Renderer/VertexDecleration.h"
#include "./Renderer/Shader.h"
#include "./Utilities/Log.h"


const std::wstring RenderPass::SHADER_FILE_PATH = L"./Src/Shaders/";

void RenderPass::AddVertexAndPixelShader(
	D3D::DevicePtr&             device,
	const char*                 name,
	const wchar_t*              vs,
	const wchar_t*              ps,
	const char*                 vs_entry,
	const char*                 ps_entry,
	const VEDType&              vedType)
{
	if (m_pShaderTable.find(name) != m_pShaderTable.end())
	{
		// テーブル上にすでに存在していた場合はスルー
		Log::Warning("[RENDER PASS] The Shader already exists. (%s)", name);
		return;
	}
	std::wstring vsFile = SHADER_FILE_PATH + std::wstring(vs);
	std::wstring psFile = SHADER_FILE_PATH + std::wstring(ps);


	m_pShaderTable[name] = std::make_shared<Shader>();
	m_pShaderTable[name]->createShader(device, vsFile, psFile, vs_entry, ps_entry, vedType);
}

void RenderPass::AddGeometryShader(D3D::DevicePtr& device, const char* name, const wchar_t* gs, const char* gs_entry)
{
	if (m_pShaderTable.find(name) == m_pShaderTable.end())
	{
		// テーブル上に同じ名前の頂点ピクセルシェーダがない場合、スルー
		Log::Warning("[RENDER PASS] Vertex Shader and Pixel Shader doesn't exist. (%s)", name);
		return;
	}

	std::wstring gsFile = SHADER_FILE_PATH + std::wstring(gs);
	
	m_pShaderTable[name]->createGeometryShader(device, gsFile, gs_entry);
}

void RenderPass::AddDomainAndHullShader(D3D::DevicePtr& device, const char* name, const wchar_t* ds, const wchar_t* hs, const char* ds_entry, const char* hs_entry)
{
	if (m_pShaderTable.find(name) == m_pShaderTable.end())
	{
		// テーブル上に同じ名前の頂点ピクセルシェーダがない場合、スルー
		Log::Warning("[RENDER PASS] Vertex Shader and Pixel Shader doesn't exist. (%s)", name);
		return;
	}

	
	std::wstring hsFile = SHADER_FILE_PATH + std::wstring(hs);

	m_pShaderTable[name]->createHullAndDomain(device, hsFile, hs_entry, ds_entry);
}

std::shared_ptr<Shader>& RenderPass::GetShaderPtr(const std::string& key)
{
	auto it = m_pShaderTable.find(key);
	if (it != m_pShaderTable.end())
	{
		return it->second;
	}
}
