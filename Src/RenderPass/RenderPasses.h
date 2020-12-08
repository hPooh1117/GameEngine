#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <map>

#include "ShaderIDTable.h"

#include "./Renderer/D3D_Helper.h"

class DepthStencilView;
class Shader;
class RenderTarget;
class NewMeshRenderer;
class GraphicsEngine;

enum class VEDType : unsigned int;


enum RenderPassID
{
	EForwardPass,
	EShadowPass,
	EDeferredPass,
	ESSAOPass,
	EPostEffectPass,
	ECubeMapPass,

	ENUM_PASS_MAX,
};




class RenderPass
{
public:
	static const std::string SHADER_NAME_TABLE[ShaderID::ENUM_SHADER_MAX];
	static const std::wstring SHADER_FILE_PATH;

private:
	static std::unique_ptr<RenderTarget>                   mpRenderTargetManager;

protected:
	static int										       mCurrentScreenNum;
	static bool 									       mbIsOpen2ndScreen;
	static bool											   mbShowsResult;
	static UINT											   mChosenRenderTarget;


	std::unordered_map<UINT, std::unique_ptr<Shader>>      mpShaderTable;
	std::unordered_map<UINT, std::wstring>                 mpShaderNameTable;
	//std::unique_ptr<RenderTarget>						   mpRenderTargets;
	std::unique_ptr<DepthStencilView>                      mpDSV;
	bool                                                   mbIsInitialized;
	bool												   mbWannaReloadShader;
	UINT												   mChosenShaderID;
public:
	RenderPass();
	virtual ~RenderPass() = default;

	virtual void Initialize(D3D::DevicePtr& device) = 0;

protected:
	void InitializeCommonShader(D3D::DevicePtr& device);

	void AddVertexAndPixelShader(
		D3D::DevicePtr& device,
		UINT           id,
		const wchar_t* vs,
		const wchar_t* ps,
		const char* vs_entry,
		const char* ps_entry,
		const VEDType& vedType);

	void AddGeometryShader(
		D3D::DevicePtr& device,
		UINT           id,
		const wchar_t* gs,
		const char* gs_entry);

	void AddDomainAndHullShader(
		D3D::DevicePtr& device,
		UINT           id,
		const wchar_t* ds,
		const wchar_t* hs,
		const char* ds_entry,
		const char* hs_entry);
	
	//void CreateRenderTarget(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT rt_id);
	//void CreateCubeRenderTarget(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT mip_slice, UINT rt_id);
	//// minIDà»ç~ÇÃnum_activateÇÃêîÇæÇØRenderTargetÇActivate
	//void ActivateRenderTargets(D3D::DeviceContextPtr& p_imm_context, UINT num_activate = 1, UINT min_rt_ID = 0);
	//// minIDà»ç~ÇÃnum_activateÇÃêîÇæÇØRenderTargetÇSRVÇ∆ÇµÇƒSet
	//void SetRenderTargetsForSRV(D3D::DeviceContextPtr& p_imm_context, UINT num_activate = 1, UINT min_rt_ID = 0, UINT slot = 0);

	
public:
	Shader*   GetShaderPtr(UINT shader_id);
	auto&     GetRenderTargetManager() { return mpRenderTargetManager; }
	
	//const std::unique_ptr<RenderTarget>& GetRenderTargetPtr() { return mpRenderTargets; }
	//RenderTarget* GetRenderTargetPtr(UINT id) { if (id < 0 || id >= mpRenderTargetTable.size()) return nullptr; return mpRenderTargetTable[id].get(); }

	bool IsInitialized() { return mbIsInitialized; }

	void SetShader(D3D::DeviceContextPtr& p_imm_context, UINT id);
	void CheckActivatedShaders();
	void ShowShaderList(std::unique_ptr<GraphicsEngine>& p_graphics, const char* current_pass);
	void ReloadShaderFile(std::unique_ptr<GraphicsEngine>& p_graphics);
	static void Clear();
	//void SetBackBuffer(std::unique_ptr<GraphicsEngine>& p_graphics);
};