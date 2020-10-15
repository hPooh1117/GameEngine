#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include "./Renderer/D3D_Helper.h"

class Shader;
class MeshRenderer;
enum class VEDType;

class RenderPass
{
public:
	static const std::wstring SHADER_FILE_PATH;

protected:
	std::unordered_map<std::string, std::shared_ptr<Shader>> m_pShaderTable;
	std::shared_ptr<MeshRenderer>                            m_pMeshRenderer;

public:
	RenderPass() = default;
	virtual ~RenderPass() = default;

	virtual void Initialize(D3D::DevicePtr& device) = 0;

	void AddVertexAndPixelShader(
		D3D::DevicePtr& device,
		const char* name,
		const wchar_t* vs,
		const wchar_t* ps,
		const char* vs_entry,
		const char* ps_entry,
		const VEDType& vedType);

	void AddGeometryShader(
		D3D::DevicePtr& device,
		const char* name,
		const wchar_t* gs,
		const char* gs_entry);

	void AddDomainAndHullShader(
		D3D::DevicePtr& device,
		const char* name,
		const wchar_t* ds,
		const wchar_t* hs,
		const char* ds_entry,
		const char* hs_entry);

	std::shared_ptr<Shader>& GetShaderPtr(const std::string& key);
};