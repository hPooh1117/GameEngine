#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <memory>
#include <vector>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DirectXTK.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DirectXTK.lib")
#endif

#include "D3D_Helper.h"

struct ShaderMacro;


class ResourceManager
{
public:
    static HRESULT LoadTexFile(
        Microsoft::WRL::ComPtr<ID3D11Device>& device, 
        const wchar_t*,
        ID3D11ShaderResourceView**,
        D3D11_TEXTURE2D_DESC*);

    static HRESULT CreateVSFromFile(
        Microsoft::WRL::ComPtr<ID3D11Device>& device, 
        const char*, ID3D11VertexShader**,
        ID3D11InputLayout**, 
        D3D11_INPUT_ELEMENT_DESC*,
        UINT);
    static HRESULT CreateVSFromFile(
        D3D::DevicePtr& device,
        const std::string& vs_name,
        D3D::VSPtr& vs
    );

    static HRESULT CompileHLSLFile(
        const std::wstring& filename,
        const std::string& func, 
        const std::string shaderModel,
        Microsoft::WRL::ComPtr<ID3DBlob>& shaderBlob);
    static HRESULT CompileHLSLFile(
        const wchar_t* filename,
        const char* func,
        const char* shader_model,
        D3D::BlobPtr& shader_blob,
        const std::vector<ShaderMacro>& shader_macros );
    

    //static HRESULT FetchDataFromCSO(
    //    const std::wstring& filename,
    //    std::shared_ptr<wchar_t[]>& data,
    //    unsigned int& size
    //);

    

    static HRESULT CreatePSFromFile(
        Microsoft::WRL::ComPtr<ID3D11Device>& device, 
        const char*,
        ID3D11PixelShader**);

    static HRESULT CreatePSFromFile(
        D3D::DevicePtr& device,
        const std::string& ps_name,
        D3D::PSPtr& ps
    );


    static HRESULT CreateFilenameToRefer(
        wchar_t(&newFilename)[256], 
        const wchar_t*, 
        const wchar_t*,
        bool will_be_deleted = true);

    static HRESULT CreateDummyTexture(
        D3D::DevicePtr& device,
        D3D::SRVPtr& p_srv);
};

