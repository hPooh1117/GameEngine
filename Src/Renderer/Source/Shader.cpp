#include "Shader.h"

#include <memory>
#include <d3dcompiler.h>
#include <functional>

#include "ResourceManager.h"
#include "VertexDecleration.h"

#include "./Utilities/Log.h"
#include "./Utilities/misc.h"
#include "./Utilities/ImGuiSelf.h"




const char* Shader::SHADER_MODEL_TABLE[Graphics::SHADER_STAGE_MAX] = {
	"",
	"vs_5_0",
	"ps_5_0",
	"hs_5_0",
	"ds_5_0",
	"gs_5_0",
	"cs_5_0",
};

Shader::Shader()
	:mbIsActivated(false)
{
}

#pragma region Old_Impl

// @brief compile hlsl files and Create shader object
bool Shader::createShader(
	D3D::DevicePtr& device,
	std::wstring vs_filename,
	std::wstring ps_filename,
	std::string vs_funcname,
	std::string ps_funcname,
	VEDType type
)
{
	HRESULT result = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;


	//// compile and Create vertex shader
	result = ResourceManager::CompileHLSLFile(vs_filename, vs_funcname, SHADER_MODEL_TABLE[Graphics::EVertexShader], vsBlob);
	if (FAILED(result))
	{
		char vsfile_multibyte[256];
		WideCharToMultiByte(CP_ACP, 0, vs_filename.c_str(), -1, vsfile_multibyte, 256, NULL, NULL);


		Log::Error("Couldn't Compile Vertex Shader ( %s )", vsfile_multibyte);
	}

	result = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, mpVS.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

	
	// Create input layouts
	VertexDecleration::CreateInputElements(device, mpInputLayout, vsBlob, type);


	// compile and Create pixel shader
	result = ResourceManager::CompileHLSLFile(ps_filename, ps_funcname, SHADER_MODEL_TABLE[Graphics::EPixelShader], psBlob);
	if (FAILED(result))
	{
		char psfile_multibyte[256];
		WideCharToMultiByte(CP_ACP, 0, ps_filename.c_str(), -1, psfile_multibyte, 256, NULL, NULL);

		Log::Error("Couldn't Compile Pixel Shader ( %s )", psfile_multibyte);
	}

	result = device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		mpPS.GetAddressOf()
	);
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

	mFuncNameTable.emplace(Graphics::EVertexShader, vs_funcname);
	mFuncNameTable.emplace(Graphics::EPixelShader, ps_funcname);


	return true;
}

bool Shader::CreateGeometryShader(
	D3D::DevicePtr& device,
	std::wstring gs_name,
	std::string gs_func)
{
	HRESULT result = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> gsBlob = nullptr;

	result = ResourceManager::CompileHLSLFile(gs_name, gs_func, SHADER_MODEL_TABLE[Graphics::EGeometryShader], gsBlob);
	device->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), NULL, mpGS.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

	mFuncNameTable.emplace(Graphics::EGeometryShader, gs_func);

	return true;
}

bool Shader::CreateHullAndDomain(
	D3D::DevicePtr& device,
	std::wstring hlsl_name,
	std::string hs_func,
	std::string ds_func)
{
	HRESULT result = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> hsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> dsBlob = nullptr;

	result = ResourceManager::CompileHLSLFile(hlsl_name, hs_func, SHADER_MODEL_TABLE[Graphics::EHullShader], hsBlob);
	device->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), NULL, mpHS.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

	result = ResourceManager::CompileHLSLFile(hlsl_name, ds_func, SHADER_MODEL_TABLE[Graphics::EDomainShader], dsBlob);
	device->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), NULL, mpDS.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

	mFuncNameTable.emplace(Graphics::EHullShader, hs_func);
	mFuncNameTable.emplace(Graphics::EDomainShader, ds_func);

	return true;
}

bool Shader::CreateComputeShader(
	D3D::DevicePtr& device,
	const std::wstring& hlsl_name,
	const std::string& cs_func)
{
	auto result = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> csBlob = nullptr;

	result = ResourceManager::CompileHLSLFile(hlsl_name, cs_func, SHADER_MODEL_TABLE[Graphics::EComputeShader], csBlob);
	device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), NULL, mpCS.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));


	mFuncNameTable.emplace(Graphics::EComputeShader, cs_func);

	return true;
}

bool Shader::CreateComputeShader(
	D3D::DevicePtr& device,
	const wchar_t* hlsl_name,
	const char* cs_func,
	const std::vector<ShaderMacro>& shader_macros)
{
	auto result = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> csBlob = nullptr;


	result = ResourceManager::CompileHLSLFile(hlsl_name, cs_func, SHADER_MODEL_TABLE[Graphics::EComputeShader], csBlob, shader_macros);
	device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), NULL, mpCS.GetAddressOf());
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));


	mFuncNameTable.emplace(Graphics::EComputeShader, cs_func);

	return true;
}

bool Shader::CreateShader(
	D3D::DevicePtr& device,
	const std::wstring& vs_name,
	const std::wstring& ps_name,
	VEDType type)
{
	HRESULT result = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;

	result = D3DReadFileToBlob(vs_name.c_str(), vsBlob.GetAddressOf());
	result = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, mpVS.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[SHADER] Couldn't Create Vertex Shader.");
		return false;
	}

	VertexDecleration::CreateInputElements(device, mpInputLayout, vsBlob, type);

	result = D3DReadFileToBlob(ps_name.c_str(), psBlob.GetAddressOf());
	result = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, mpPS.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[SHADER] Couldn't Create Pixel Shader.");
		return false;
	}

	Log::Info("[SHADER] Create Vertex Shader.");
	Log::Info("[SHADER] Create Pixel Shader.");

	return true;
}

#pragma endregion

bool Shader::CreateShader(
	D3D::DevicePtr& device, 
	UINT shader_type, 
	const std::wstring& hlsl,
	const char* func,
	const VEDType& ved_type)
{
	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	if (FAILED(ResourceManager::CompileHLSLFile(hlsl, func, SHADER_MODEL_TABLE[shader_type], blob)))
	{
		Log::Error("Couldn't compile.");
		return false;
	}

	switch (shader_type)
	{
	case Graphics::EVertexShader:   CreateVertexShader(device, blob);   break;
	case Graphics::EPixelShader:    CreatePixelShader(device, blob);    mFuncNameTable.emplace(shader_type, func); return true;
	case Graphics::EHullShader:     CreateHullShader(device, blob);     mFuncNameTable.emplace(shader_type, func); return true;
	case Graphics::EDomainShader:   CreateDomainShader(device, blob);   mFuncNameTable.emplace(shader_type, func); return true;
	case Graphics::EGeometryShader: CreateGeometryShader(device, blob); mFuncNameTable.emplace(shader_type, func); return true;
	case Graphics::EComputeShader:  CreateComputeShader(device, blob);  mFuncNameTable.emplace(shader_type, func); return true;
	default: return false;
	}


	if (mpVS) VertexDecleration::CreateInputElements(device, mpInputLayout, blob, ved_type);
	mVEDType = ved_type;
	mFuncNameTable.emplace(shader_type, func);

	return true;
}


// 既にロードされたシェーダを編集する目的。ランタイム中の新規作成は想定しない。
bool Shader::ReloadShader(D3D::DevicePtr& device, const std::wstring& hlsl)
{
	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	UINT type = ChooseShaderType();

	if (type == Graphics::ENoneShader)     return false;
	if (type == Graphics::SHADER_STAGE_MAX) return true;

	// 指定した種類のシェーダが存在しなければ終了
	auto it = mFuncNameTable.find(type);
	if (it == mFuncNameTable.end())
	{
		Log::Error("Referred shader type doesn't exist");
		return true;
	}

	Log::Info("Start re-compiling");

	auto result = ResourceManager::CompileHLSLFile(hlsl, mFuncNameTable.at(type), SHADER_MODEL_TABLE[type], blob);
	if (FAILED(result))
	{
		Log::Error("Couldn't compile.");
		return true;
	}


	switch (type)
	{
	case Graphics::EVertexShader:   CreateVertexShader(device, blob);   break;
	case Graphics::EPixelShader:    CreatePixelShader(device, blob);    return true;
	case Graphics::EHullShader:     CreateHullShader(device, blob);     return true;
	case Graphics::EDomainShader:   CreateDomainShader(device, blob);   return true;
	case Graphics::EGeometryShader: CreateGeometryShader(device, blob); return true;
	case Graphics::EComputeShader:  CreateComputeShader(device, blob);  return true;
	default: return true;
	}

	if (mpVS) VertexDecleration::CreateInputElements(device, mpInputLayout, blob, mVEDType);
	return true;
}

//void Shader::ReloadShader(D3D::DevicePtr& device, const std::wstring& hlsl_name, UINT shader_type)
//{
//	Microsoft::WRL::ComPtr<ID3DBlob> blob;
//
//	// 指定した種類のシェーダが存在しなければ終了
//	auto it = mFuncNameTable.find(shader_type);
//	if (it == mFuncNameTable.end())
//	{
//		Log::Error("Referred shader type doesn't exist");
//		return;
//	}
//
//	auto result = ResourceManager::CompileHLSLFile(hlsl_name, mFuncNameTable.at(shader_type), SHADER_MODEL_TABLE[shader_type], blob);
//	if (FAILED(result))
//	{
//		Log::Error("Couldn't compile.");
//		return;
//	}
//	
//
//	switch (shader_type)
//	{
//	case EPixelShader:    CreatePixelShader(device, blob);    break;
//	case EHullShader:     CreateHullShader(device, blob);     break;
//	case EDomainShader:   CreateDomainShader(device, blob);   break;
//	case EGeometryShader: CreateGeometryShader(device, blob); break;
//	case EComputeShader:  CreateComputeShader(device, blob);  break;
//	default:
//	}
//
//}

void Shader::CreateVertexShader(D3D::DevicePtr& device, D3D::BlobPtr& blob)
{
	D3D::VSPtr vs = nullptr;
	auto result = device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, vs.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("Couldn't create VERTEX shader.");
		mbCreated[Graphics::EVertexShader] = false;

		return;
	}
	mpVS = vs;
	mbCreated[Graphics::EVertexShader] = true;
}

void Shader::CreateHullShader(D3D::DevicePtr& device, D3D::BlobPtr& blob)
{
	D3D::HSPtr hs = nullptr;
	auto result = device->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, hs.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("Couldn't create HULL shader.");
		return;
	}
	mpHS = hs;
}

void Shader::CreateDomainShader(D3D::DevicePtr& device, D3D::BlobPtr& blob)
{
	D3D::DSPtr ds = nullptr;
	auto result = device->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, ds.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("Couldn't create DOMAIN shader.");
		return;
	}
	mpDS = ds;
}

void Shader::CreateGeometryShader(D3D::DevicePtr& device, D3D::BlobPtr& blob)
{
	D3D::GSPtr gs = nullptr;
	auto result = device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, gs.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("Couldn't create GEOMETRY shader.");
		return;
	}
	mpGS = gs;
}

void Shader::CreatePixelShader(D3D::DevicePtr& device, D3D::BlobPtr& blob)
{
	D3D::PSPtr ps = nullptr;
	auto result = device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, ps.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("Couldn't create PIXEL shader.");
		mbCreated[Graphics::EPixelShader] = false;
		return;
	}
	mpPS = ps;
	mbCreated[Graphics::EPixelShader] = true;
}

void Shader::CreateComputeShader(D3D::DevicePtr& device, D3D::BlobPtr& blob)
{
	D3D::CSPtr cs = nullptr;
	auto result = device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, cs.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("Couldn't create COMPUTE shader.");
		return;
	}
	mpCS = cs;
}

UINT Shader::ChooseShaderType()
{
	static int item = 0;
	int chosen = 0;

	ImGui::OpenPopup("Reload Shader?");

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Reload Shader?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::Combo("--Shader Type--", &item, "Please select\0Vertex Shader\0Pixel Shader\0Geometry Shader\0Domain Shader\0HullShader\0ComputeShader\0\0\0"))
		{
			Log::Info("%d", item);
		}
		//mpShaderTable.at(mChosenShaderID)->ReloadShader(p_graphics->GetDevicePtr(), mpShaderNameTable.at(mChosenShaderID), EVertexShader);
		if (ImGui::Button("Reload"))
		{
			ImGui::CloseCurrentPopup();
			chosen = item;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
			chosen = Graphics::SHADER_STAGE_MAX;
		}
		ImGui::EndPopup();
	}

	return chosen;
}



void Shader::ActivateShaders(D3D::DeviceContextPtr& p_imm_context)
{
	mbIsActivated = true;
	p_imm_context->IASetInputLayout(mpInputLayout.Get());
	p_imm_context->VSSetShader(mpVS.Get(), nullptr, 0);
	p_imm_context->HSSetShader(mpHS.Get(), nullptr, 0);
	p_imm_context->DSSetShader(mpDS.Get(), nullptr, 0);
	p_imm_context->GSSetShader(mpGS.Get(), nullptr, 0);
	p_imm_context->PSSetShader(mpPS.Get(), nullptr, 0);

}

void Shader::ActivateCSShader(D3D::DeviceContextPtr& p_imm_context)
{
	p_imm_context->CSSetShader(mpCS.Get(), NULL, 0);
}

void Shader::DeactivateCSShader(D3D::DeviceContextPtr& p_imm_context)
{
	p_imm_context->CSSetShader(nullptr, NULL, 0);
}

Shader::~Shader()
{
}
