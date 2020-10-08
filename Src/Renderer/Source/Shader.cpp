#include "Shader.h"
#include <d3dcompiler.h>
#include "ResourceManager.h"
#include "VertexDecleration.h"

#pragma comment(lib, "d3dcompiler.lib")

Shader::Shader()
{
}

// @brief compile hlsl files and create shader object
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


    //// compile and create vertex shader
    result = ResourceManager::compileHLSLFile(vs_filename, vs_funcname, "vs_5_0", vsBlob);
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, m_vs.GetAddressOf());

    // create input layouts
    VertexDecleration::CreateInputElements(device, m_input_layout, vsBlob, type);


    // compile and create pixel shader
    result = ResourceManager::compileHLSLFile(ps_filename, ps_funcname, "ps_5_0", psBlob);
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

    return true;
}

bool Shader::createGeometryShader(
    Microsoft::WRL::ComPtr<ID3D11Device>& device, 
    std::wstring gs_name,
    std::string gs_func)
{
    HRESULT result = S_OK;

    Microsoft::WRL::ComPtr<ID3DBlob> gsBlob = nullptr;

    result = ResourceManager::compileHLSLFile(gs_name, gs_func, "gs_5_0", gsBlob);
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

    result = ResourceManager::compileHLSLFile(hlsl_name, hs_func, "hs_5_0", hsBlob);
    device->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), NULL, m_hs.GetAddressOf());

    result = ResourceManager::compileHLSLFile(hlsl_name, ds_func, "ds_5_0", dsBlob);
    device->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), NULL, m_ds.GetAddressOf());

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
