#include "ResourceManager.h"

#include <map>
#include <wrl/client.h>
#include <memory>
#include "WICTextureLoader.h"
#include <d3dcompiler.h>
#include <unordered_map>
#include <tuple>


#include "./Utilities/misc.h"
#include "DirectXTex.h"


// @brief Reuse ShaderResourceView & texture2d desc for same texture file having been used
// @param ID3D11Device               device
// @param const wchar_t*             filename : name of texture file
// @param ID3D11ShaderResourceView** srView
// @param D3D11_TEXTURE2D_DESC*      desc
HRESULT ResourceManager::LoadTexFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t* filename, ID3D11ShaderResourceView** srView, D3D11_TEXTURE2D_DESC* desc)
{
    auto hr = S_OK;
    Microsoft::WRL::ComPtr<ID3D11Resource> buffer;

    static std::map<const std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;
    auto it = cache.find(filename);
    if (it != cache.end())
    {
        (*srView) = it->second.Get();
        (*srView)->AddRef();
        (*srView)->GetResource(buffer.GetAddressOf());
    }
    else
    {
        std::wstring texFile(filename);
        std::wstring extension = texFile.substr(texFile.size() - 3);

        DirectX::TexMetadata metadata = {};
        DirectX::ScratchImage image = {};

        if (extension == L"tga")
        {
            hr = DirectX::LoadFromTGAFile(texFile.c_str(), &metadata, image);
            _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));


        }
        else
        {
            hr = DirectX::LoadFromWICFile(texFile.c_str(), 0, &metadata, image);
            //hr = DirectX::CreateWICTextureFromFile(device.Get(), filename, buffer.GetAddressOf(), srView);
            _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

        }
        hr = DirectX::CreateShaderResourceView(
            device.Get(),
            image.GetImages(),
            image.GetImageCount(),
            metadata,
            srView
        );
        _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

        (*desc).Width = metadata.width;
        (*desc).Height = metadata.height;
        

        cache.insert(std::make_pair(filename, *srView));
    }

    //Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
    //hr = buffer.Get()->QueryInterface(texture2D.GetAddressOf());
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    //texture2D->GetDesc(desc);

    return hr;
}



// @brief Reuse ID3D11VertexShader and ID3D11InputLayout having been used
// @param ID3D11Device         device
// @param const char*          csoname : name of csofile
// @param ID3D11VertexShader** vs
// @param ID3D11InputLayout**  inputLayout
// @param D3D11_ELEMENT_DESC*  desc
// @param UINT                 numElement : size of input element desc
HRESULT ResourceManager::CreateVSFromFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
    const char* csoname,
    ID3D11VertexShader** vs,
    ID3D11InputLayout** inputLayout,
    D3D11_INPUT_ELEMENT_DESC* desc,
    UINT numElement)
{
    struct FragmentForVS
    {
        FragmentForVS(ID3D11VertexShader** vertexShader, ID3D11InputLayout** input) :mVS(*vertexShader), mInput(*input) {}
        Microsoft::WRL::ComPtr<ID3D11VertexShader> mVS;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> mInput;
    };

    static std::map<std::string, FragmentForVS> cache;

    // search another fragment
    auto it = cache.find(csoname);
    if (it != cache.end())
    {
        *vs = it->second.mVS.Get();
        (*vs)->AddRef();
        *inputLayout = it->second.mInput.Get();
        (*inputLayout)->AddRef();
        return S_OK;
    }

    // if another fragment doesn't exist, open csoFile.
    // 頂点シェーダーのHLSLファイルの入力
    FILE* fp = nullptr;
    fopen_s(&fp, csoname, "rb");
    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    std::unique_ptr<unsigned char[]> csoData;
    csoData = std::make_unique<unsigned char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);



    HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vs);
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    hr = device->CreateInputLayout(desc, numElement, csoData.get(), csoSize, inputLayout);
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    // insert new element
    cache.insert(std::make_pair(csoname, FragmentForVS(vs, inputLayout)));

    return hr;
}


struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ h2;
    }
};

HRESULT ResourceManager::compileHLSLFile(
    std::wstring & filename,
    std::string & func, 
    std::string shaderModel,
    Microsoft::WRL::ComPtr<ID3DBlob>& shaderBlob)
{
    typedef std::pair <std::wstring, std::string> Keys;
    static std::unordered_map < Keys, Microsoft::WRL::ComPtr<ID3DBlob>, pair_hash> cache;

    Keys key = std::make_pair(filename, func);

    auto it = cache.find(key);
    if (it != cache.end())
    {
        shaderBlob = it->second;
        return S_OK;
    }

    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    shaderFlags |= D3DCOMPILE_DEBUG;
    shaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    HRESULT result = D3DCompileFromFile(
        filename.c_str(),
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        func.c_str(),
        shaderModel.c_str(),
        shaderFlags,
        0,
        shaderBlob.GetAddressOf(),
        errorBlob.GetAddressOf()
        );

    if (errorBlob != NULL)
    {
        MessageBoxA(0, (char*)errorBlob->GetBufferPointer(), NULL, MB_OK);
    }

    cache.emplace(key, shaderBlob);

    return result;

}




// @brief Reuse PixelShader having been used
// @param ID3D11Device         device
// @param const char*          csoname : name of csofile
// @param ID3D11PixelShader** vs
HRESULT ResourceManager::CreatePSFromFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const char* csoname, ID3D11PixelShader** ps)
{
    static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> cache;
    auto it = cache.find(csoname);

    if (it != cache.end())
    {
        *ps = it->second.Get();
        (*ps)->AddRef();
        return S_OK;
    }

    // ピクセルシェーダーのHLSLファイルの入力
    FILE* fp = nullptr;
    fopen_s(&fp, csoname, "rb");
    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, ps);
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    cache.insert(std::make_pair(csoname, *ps));

    return hr;
}


// @brief Bind the PATH that reffererFilename has TO targetFilename
// @param wchar_t*        newFilename       : Output;new filename
// @param const wchar_t*  reffererFilename  : it has PATH that we need
// @param const wchar_t*  targetFilename    : want to make this bind PATH
HRESULT ResourceManager::CreateFilenameToRefer(wchar_t(&newFilename)[256], const wchar_t* reffererFilename, const wchar_t* targetFilename)
{
    HRESULT hr = S_OK;
    const wchar_t delimiters[] = { L'\\', L'/' };

    wchar_t directory[256] = {};
    for (wchar_t delimiter : delimiters)
    {
        wchar_t *p = wcsrchr(const_cast<wchar_t*>(reffererFilename), delimiter);
        if (p)
        {
            memcpy_s(directory, 256, reffererFilename, (p - reffererFilename + 1) * sizeof(wchar_t));
            break;
        }
    }
    wchar_t filename[256];
    wcscpy_s(filename, targetFilename);
    for (wchar_t delimiter : delimiters)
    {
        wchar_t *p = wcsrchr(filename, delimiter);
        if (p)
        {
            wcscpy_s(filename, p + 1);
            break;
        }
    }
    wcscpy_s(newFilename, directory);
    wcscat_s(newFilename, filename);
    return hr;
}

// @brief Make dummy texture corresponding with the texture-less fbx mesh 
// @param ID3D11Device*              device
// @param ID3D11ShaderResourceView*  shader_resource_view
HRESULT ResourceManager::CreateDummyTexture(
    D3D::DevicePtr& device,
    D3D::ShaderResouceVPtr& p_srv)
{
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC texture2d_desc = {};
    texture2d_desc.Width = 1;
    texture2d_desc.Height = 1;
    texture2d_desc.MipLevels = 1;
    texture2d_desc.ArraySize = 1;
    texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2d_desc.SampleDesc.Count = 1;
    texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texture2d_desc.CPUAccessFlags = 0;
    texture2d_desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subresource_data = {};
    u_int color = 0xFFFFFFFF;
    subresource_data.pSysMem = &color;
    subresource_data.SysMemPitch = 4;
    subresource_data.SysMemSlicePitch = 4;

    ID3D11Texture2D *texture2d;
    hr = device->CreateTexture2D(&texture2d_desc, nullptr, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
    shader_resource_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(texture2d, &shader_resource_view_desc, p_srv.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    texture2d->Release();

    return hr;
}

