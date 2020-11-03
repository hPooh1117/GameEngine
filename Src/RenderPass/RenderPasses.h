#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include "ShaderIDTable.h"

#include "./Renderer/D3D_Helper.h"

class DepthStencilView;
class Shader;
class RenderTarget;
class NewMeshRenderer;

enum class VEDType : unsigned int;


enum RenderPassID
{
	EForwardPass,
	EShadowPass,
	EDefferedPass,
	ESSAOPass,
	EPostEffectPass,

	ENUM_PASS_MAX,
};




class RenderPass
{
public:
	static const std::string SHADER_NAME_TABLE[ShaderType::ENUM_SHADER_MAX];
	static const std::wstring SHADER_FILE_PATH;

protected:
	std::unordered_map<ShaderType, std::unique_ptr<Shader>>    mpShaderTable;
	std::unique_ptr<RenderTarget>							   mpRenderTargets;
	std::unique_ptr<DepthStencilView>                          mpDSV;
public:
	RenderPass();
	virtual ~RenderPass() = default;

	virtual void Initialize(D3D::DevicePtr& device) = 0;

protected:
	void InitializeCommonShader(D3D::DevicePtr& device);

	void AddVertexAndPixelShader(
		D3D::DevicePtr& device,
		ShaderType       id,
		const wchar_t* vs,
		const wchar_t* ps,
		const char* vs_entry,
		const char* ps_entry,
		const VEDType& vedType);

	void AddGeometryShader(
		D3D::DevicePtr& device,
		ShaderType       id,
		const wchar_t* gs,
		const char* gs_entry);

	void AddDomainAndHullShader(
		D3D::DevicePtr& device,
		ShaderType       id,
		const wchar_t* ds,
		const wchar_t* hs,
		const char* ds_entry,
		const char* hs_entry);

	
public:
	const std::unique_ptr<Shader>&          GetShaderPtr(ShaderType id);
	const std::unique_ptr<RenderTarget>& GetRenderTargetPtr() { return mpRenderTargets; }

	void SetShader(D3D::DeviceContextPtr& p_imm_context, ShaderType id);
	//void SetBackBuffer(std::unique_ptr<GraphicsEngine>& p_graphics);
};