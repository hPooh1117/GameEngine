#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <string>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DirectXTK.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DirectXTK.lib")
#endif

#include "D3D_Helper.h"


// @class ResourceManager
// @brief to sharing texture resource among the sprite classes using same texture.
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

    static HRESULT compileHLSLFile(
        std::wstring& filename,
        std::string& func, 
        std::string shaderModel,
        Microsoft::WRL::ComPtr<ID3DBlob>& shaderBlob);

    static HRESULT CreatePSFromFile(
        Microsoft::WRL::ComPtr<ID3D11Device>& device, 
        const char*,
        ID3D11PixelShader**);


    static HRESULT CreateFilenameToRefer(
        wchar_t(&newFilename)[256], 
        const wchar_t*, const wchar_t*);

    static HRESULT CreateDummyTexture(
        D3D::DevicePtr& device,
        D3D::ShaderResouceVPtr& p_srv);
};

