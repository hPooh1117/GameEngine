#pragma once
#include <unordered_map>

#include "D3D_Helper.h"
#include "VertexDecleration.h"

enum ShaderType
{
    ENoneShader,
    EVertexShader,
    EPixelShader,
    EHullShader,
    EDomainShader,
    EGeometryShader,
    EComputeShader,
    
    SHADER_TYPE_MAX,
};

struct ShaderMacro
{
    std::string name;
    std::string value;
};

struct ShaderStageDesc
{
    std::wstring filename;
    std::vector<ShaderMacro> macros;
};

class Shader
{
private:
    static const char* SHADER_MODEL_TABLE[SHADER_TYPE_MAX];

    D3D::VSPtr              mpVS = nullptr;
    D3D::HSPtr              mpHS = nullptr;
    D3D::DSPtr              mpDS = nullptr;
    D3D::GSPtr              mpGS = nullptr;
    D3D::PSPtr              mpPS = nullptr;
    D3D::CSPtr              mpCS = nullptr;
    D3D::InputLayoutPtr     mpInputLayout = nullptr;
    bool mbIsActivated;
    bool mbCreated[2];
    VEDType                 mVEDType;
    std::unordered_map<UINT, std::string>  mFuncNameTable;


public:
    Shader();
    ~Shader();

#pragma region Old_Impl
    bool createShader(
        D3D::DevicePtr& device,
        std::wstring vs_name,
        std::wstring ps_name,
        std::string vs_func,
        std::string ps_func,
        VEDType type
        );
    bool CreateGeometryShader(
        D3D::DevicePtr& device,
        std::wstring gs_name,
        std::string gs_func
    );

    bool CreateHullAndDomain(
        D3D::DevicePtr& device,
        std::wstring hlsl_name,
        std::string hs_func,
        std::string ds_func
    );

    bool CreateComputeShader(
        D3D::DevicePtr& device,
        const std::wstring& hlsl_name,
        const std::string& cs_func
    );
    bool CreateComputeShader(
        D3D::DevicePtr& device,
        const wchar_t* hlsl_name,
        const char* cs_func,
        const std::vector<ShaderMacro>& shader_macros);


    bool CreateShader(
        D3D::DevicePtr& device,
        const std::wstring& vs_name,
        const std::wstring& ps_name,
        VEDType type
        );
#pragma endregion
    bool CreateShader(
        D3D::DevicePtr& device,
        UINT shader_type,
        const std::wstring& hlsl,
        const char* func,
        const VEDType& ved_type = VEDType::VED_NONE);
    //bool CreateShader(
    //    D3D::DevicePtr& device,
    //    UINT shader_type,
    //    const std::wstring& hlsl,
    //    const std::string& func);

    bool ReloadShader(
        D3D::DevicePtr& device,
        const std::wstring& hlsl);
    //void ReloadShader(
    //    D3D::DevicePtr& device,
    //    const std::wstring& hlsl_name,
    //    UINT shader_type);

private:
    void CreateVertexShader(D3D::DevicePtr& device,   D3D::BlobPtr& blob);
    void CreateHullShader(D3D::DevicePtr& device,     D3D::BlobPtr& blob);
    void CreateDomainShader(D3D::DevicePtr& device,   D3D::BlobPtr& blob);
    void CreateGeometryShader(D3D::DevicePtr& device, D3D::BlobPtr& blob);
    void CreatePixelShader(D3D::DevicePtr& device,    D3D::BlobPtr& blob);
    void CreateComputeShader(D3D::DevicePtr& device,  D3D::BlobPtr& blob);
    UINT ChooseShaderType();

public:
    void ActivateShaders(D3D::DeviceContextPtr& p_imm_context);
    void ActivateCSShader(D3D::DeviceContextPtr& p_imm_context);
    void DeactivateCSShader(D3D::DeviceContextPtr& p_imm_context);

    bool GetIsActivated() { return mbIsActivated; }
};