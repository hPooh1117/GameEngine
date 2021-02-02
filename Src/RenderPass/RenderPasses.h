#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <map>

#include "ShaderIDTable.h"

#include "./Renderer/D3D_Helper.h"
#include "./Renderer/GraphicsDevice.h"
#include "./Renderer/DepthStencilView.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/RenderTarget.h"
#include "./Renderer/Shader.h"

//class DepthStencilView;
//class Shader;
//class RenderTarget;
//class NewMeshRenderer;

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

	virtual void Initialize(Graphics::GraphicsDevice* device) = 0;

protected:
	void InitializeCommonShader(Graphics::GraphicsDevice* device);

	void AddVertexAndPixelShader(
		Graphics::GraphicsDevice* device,
		UINT           id,
		const wchar_t* vs,
		const wchar_t* ps,
		const char* vs_entry,
		const char* ps_entry,
		const VEDType& vedType);

	void AddGeometryShader(
		Graphics::GraphicsDevice* device,
		UINT           id,
		const wchar_t* gs,
		const char* gs_entry);

	void AddDomainAndHullShader(
		Graphics::GraphicsDevice* device,
		UINT           id,
		const wchar_t* ds,
		const wchar_t* hs,
		const char* ds_entry,
		const char* hs_entry);

	//void CreateRenderTarget(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT rt_id);
	//void CreateCubeRenderTarget(D3D::DevicePtr& p_device, UINT width, UINT height, DXGI_FORMAT format, UINT mip_slice, UINT rt_id);
	//// minID�ȍ~��num_activate�̐�����RenderTarget��Activate
	//void ActivateRenderTargets(D3D::DeviceContextPtr& p_imm_context, UINT num_activate = 1, UINT min_rt_ID = 0);
	//// minID�ȍ~��num_activate�̐�����RenderTarget��SRV�Ƃ���Set
	//void SetRenderTargetsForSRV(D3D::DeviceContextPtr& p_imm_context, UINT num_activate = 1, UINT min_rt_ID = 0, UINT slot = 0);


public:
	Shader*   GetShaderPtr(UINT shader_id);
	auto&     GetRenderTargetManager() { return mpRenderTargetManager; }

	//const std::unique_ptr<RenderTarget>& GetRenderTargetPtr() { return mpRenderTargets; }
	//RenderTarget* GetRenderTargetPtr(UINT id) { if (id < 0 || id >= mpRenderTargetTable.size()) return nullptr; return mpRenderTargetTable[id].get(); }

	bool IsInitialized() { return mbIsInitialized; }

	void SetShader(Graphics::GraphicsDevice* device, UINT id);
	void CheckActivatedShaders();
	void ShowShaderList(Graphics::GraphicsDevice* device, const char* current_pass);
	void ReloadShaderFile(Graphics::GraphicsDevice* device);
	static void Clear();
	//void SetBackBuffer(std::unique_ptr<GraphicsDevice>& p_graphics);
};
