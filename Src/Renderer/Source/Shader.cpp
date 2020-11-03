#include "Shader.h"

#include <memory>
#include <d3dcompiler.h>

#include "ResourceManager.h"
#include "VertexDecleration.h"

#include "./Utilities/Log.h"
#include "./Utilities/misc.h"
#pragma comment(lib, "d3dcompiler.lib")

Shader::Shader()
{
}

// @brief compile hlsl files and Create shader object
bool Shader::createShader(
    Microsoft::WRL::ComPtr<ID3D11Device>& device,
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
    result = ResourceManager::CompileHLSLFile(vs_filename, vs_funcname, "vs_5_0", vsBlob);
    if (FAILED(result))
    {
        char vsfile_multibyte[256];
        WideCharToMultiByte(CP_ACP, 0, vs_filename.c_str(), -1, vsfile_multibyte, 256, NULL, NULL);


        Log::Error("Couldn't Compile Vertex Shader ( %s )", vsfile_multibyte);
    }

    result = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, m_vs.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));


    // Create input layouts
    VertexDecleration::CreateInputElements(device, m_input_layout, vsBlob, type);


    // compile and Create pixel shader
    result = ResourceManager::CompileHLSLFile(ps_filename, ps_funcname, "ps_5_0", psBlob);
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
        m_ps.GetAddressOf()
        );
#ifdef DEBUG
    vsfunc = vs_funcname;
    psfunc = ps_funcname;
#endif 
    _ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

    return true;
}

bool Shader::createGeometryShader(
    Microsoft::WRL::ComPtr<ID3D11Device>& device, 
    std::wstring gs_name,
    std::string gs_func)
{
    HRESULT result = S_OK;

    Microsoft::WRL::ComPtr<ID3DBlob> gsBlob = nullptr;

    result = ResourceManager::CompileHLSLFile(gs_name, gs_func, "gs_5_0", gsBlob);
    device->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), NULL, m_gs.GetAddressOf());

    return true;
}

bool Shader::createHullAndDomain(
    Microsoft::WRL::ComPtr<ID3D11Device>& device,
    std::wstring hlsl_name, 
    std::string hs_func,
    std::string ds_func)
{
    HRESULT result = S_OK;

    Microsoft::WRL::ComPtr<ID3DBlob> hsBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> dsBlob = nullptr;

    result = ResourceManager::CompileHLSLFile(hlsl_name, hs_func, "hs_5_0", hsBlob);
    device->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), NULL, m_hs.GetAddressOf());

    result = ResourceManager::CompileHLSLFile(hlsl_name, ds_func, "ds_5_0", dsBlob);
    device->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), NULL, m_ds.GetAddressOf());

    return true;
}

bool Shader::CreateShader(
    Microsoft::WRL::ComPtr<ID3D11Device>& device, 
    const std::wstring& vs_name, 
    const std::wstring& ps_name,
    VEDType type)
{
    HRESULT result = S_OK;

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;

    result = D3DReadFileToBlob(vs_name.c_str(), vsBlob.GetAddressOf());
    result = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vs.GetAddressOf());
    if (FAILED(result))
    {
        Log::Error("[SHADER] Couldn't Create Vertex Shader." );
        return false;
    }

    VertexDecleration::CreateInputElements(device, m_input_layout, vsBlob, type);
    
    result = D3DReadFileToBlob(ps_name.c_str(), psBlob.GetAddressOf());
    result = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_ps.GetAddressOf());
    if (FAILED(result))
    {
        Log::Error("[SHADER] Couldn't Create Pixel Shader.");
        return false;
    }

    Log::Info("[SHADER] Create Vertex Shader.");
    Log::Info("[SHADER] Create Pixel Shader.");

    return true;
}


void Shader::activateShaders(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context)
{
    imm_context->IASetInputLayout(m_input_layout.Get());
    imm_context->VSSetShader(m_vs.Get(), nullptr, 0);
    imm_context->HSSetShader(m_hs.Get(), nullptr, 0);
    imm_context->DSSetShader(m_ds.Get(), nullptr, 0);
    imm_context->GSSetShader(m_gs.Get(), nullptr, 0);
    imm_context->PSSetShader(m_ps.Get(), nullptr, 0);
    
}

Shader::~Shader()
{
}
