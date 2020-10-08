#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include "VertexDecleration.h"

// @class Shader
// @brief All shader data including vertex, pixel (, geometric, hull ) are compiled here.
// @brief And then create shader object like ID3D11xxxShader.
// @param none
class Shader
{
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>    m_vs = nullptr;
    Microsoft::WRL::ComPtr<ID3D11HullShader>      m_hs = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DomainShader>    m_ds = nullptr;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>  m_gs = nullptr;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>     m_ps = nullptr;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>     m_input_layout = nullptr;
    
#ifdef _DEBUG
    std::string vsfunc;
    std::string psfunc;
#endif

public:
    Shader();
    bool createShader(
        Microsoft::WRL::ComPtr<ID3D11Device>& device,
        std::wstring vs_name,
        std::wstring ps_name,
        std::string vs_func,
        std::string ps_func,
        VEDType type
        );
    bool createGeometryShader(
        Microsoft::WRL::ComPtr<ID3D11Device>& device,
        std::wstring gs_name,
        std::string gs_func
    );

    bool createHullAndDomain(
        Microsoft::WRL::ComPtr<ID3D11Device>& device,
        std::wstring hlsl_name,
        std::string hs_func,
        std::string ds_func
    );

    void activateShaders(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context);
    ~Shader();
};