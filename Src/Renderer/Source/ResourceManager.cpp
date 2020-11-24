#include "ResourceManager.h"

#include <map>
#include <wrl/client.h>
#include "WICTextureLoader.h"
#include <d3dcompiler.h>
#include <unordered_map>
#include <tuple>
#include <fstream>
#include <iostream>
#include "DirectXTex.h"


#include "Shader.h"

#include "./Utilities/misc.h"
#include "./Utilities/Log.h"



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

		char texfile_multibyte[256];
		WideCharToMultiByte(CP_ACP, 0, texFile.c_str(), -1, texfile_multibyte, 256, NULL, NULL);


		if (extension == L"tga")
		{
			hr = DirectX::LoadFromTGAFile(texFile.c_str(), &metadata, image);
			_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
		}
		if (extension == L"hdr")
		{
			hr = DirectX::LoadFromHDRFile(texFile.c_str(), &metadata, image);
			_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			hr = DirectX::LoadFromWICFile(texFile.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image);
			if (FAILED(hr))
			{

				Log::Error("Couldn't Fetch Texture. (%s)", texfile_multibyte);
			}
			_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

		}
		hr = DirectX::CreateShaderResourceView(
			device.Get(),
			image.GetImages(),
			image.GetImageCount(),
			metadata,
			srView
		);
		if (FAILED(hr))
		{
			Log::Error("Couldn't Create Shader ResourceView. (%s)", texfile_multibyte);
		}
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

		Log::Info("[TEXTURE] Created ShaderResourceView");
		Log::Info("%s", texfile_multibyte);
		(*desc).Width = metadata.width;
		(*desc).Height = metadata.height;


		cache.insert(std::make_pair(filename, *srView));
	}
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

HRESULT ResourceManager::CreateVSFromFile(D3D::DevicePtr& device, const std::string& vs_name, D3D::VSPtr& vs)
{
	static std::map<std::string, D3D::VSPtr> cache;

	// search another fragment
	auto it = cache.find(vs_name);
	if (it != cache.end())
	{
		vs = it->second;

		return S_OK;
	}

	// if another fragment doesn't exist, open csoFile.
	// 頂点シェーダーのHLSLファイルの入力
	FILE* fp = nullptr;
	fopen_s(&fp, vs_name.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	long csoSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	std::unique_ptr<unsigned char[]> csoData;
	csoData = std::make_unique<unsigned char[]>(csoSize);
	fread(csoData.get(), csoSize, 1, fp);
	fclose(fp);



	HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vs.GetAddressOf());
	if (FAILED(hr))
	{
		Log::Error("[RESOURCE MANAGER] Couldn't Create Vertex Shader. (%s)", vs_name.c_str());
	}


	// insert new element
	cache.insert(std::make_pair(vs_name, vs));

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

HRESULT ResourceManager::CompileHLSLFile(
	const std::wstring& filename,
	const std::string& func,
	const std::string shaderModel,
	Microsoft::WRL::ComPtr<ID3DBlob>& shaderBlob)
{
	//typedef std::pair <std::wstring, std::string> Keys;
	//static std::unordered_map < Keys, Microsoft::WRL::ComPtr<ID3DBlob>, pair_hash> cache;

	//Keys key = std::make_pair(filename, func);

	//auto it = cache.find(key);
	//if (it != cache.end())
	//{
	//    shaderBlob = it->second;
	//    char file_multibyte[256];
	//    WideCharToMultiByte(CP_ACP, 0, filename.c_str(), -1, file_multibyte, 256, NULL, NULL);
	//    Log::Info("[RESOURCE MANAGER] Already compiled (%s)", file_multibyte);

	//    return S_OK;
	//}

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

	//cache.emplace(key, shaderBlob);

	char file_multibyte[256];
	WideCharToMultiByte(CP_ACP, 0, filename.c_str(), -1, file_multibyte, 256, NULL, NULL);

	Log::Info("[RESOURCE MANAGER] Finished Compiling (%s)", file_multibyte);

	return result;

}

HRESULT ResourceManager::CompileHLSLFile(
	const wchar_t* filename,
	const char* func,
	const char* shader_model,
	D3D::BlobPtr& shader_blob,
	const std::vector<ShaderMacro>& shader_macros)
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	int i = 0;
	std::vector<D3D10_SHADER_MACRO> d3dMacros(shader_macros.size() + 1);
	std::for_each(std::begin(shader_macros), std::end(shader_macros), [&](const ShaderMacro& macro)
	{
		d3dMacros[i++] = D3D10_SHADER_MACRO({ macro.name.c_str(), macro.value.c_str() });
	});
	d3dMacros[i] = { NULL, NULL };

	HRESULT result = D3DCompileFromFile(
		filename,
		d3dMacros.data(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		func,
		shader_model,
		shaderFlags,
		0,
		shader_blob.GetAddressOf(),
		errorBlob.GetAddressOf()
	);

	if (errorBlob != NULL)
	{
		MessageBoxA(0, (char*)errorBlob->GetBufferPointer(), NULL, MB_OK);

	}

	//cache.emplace(key, shaderBlob);

	char file_multibyte[256];
	WideCharToMultiByte(CP_ACP, 0, filename, -1, file_multibyte, 256, NULL, NULL);

	Log::Info("[RESOURCE MANAGER] Finished Compiling (%s)", file_multibyte);

	return result;



	return E_NOTIMPL;
}

//HRESULT ResourceManager::FetchDataFromCSO(
//    const std::wstring&                 filename,
//    std::shared_ptr<wchar_t[]>&         data, 
//    unsigned int&                       size)
//{
//    auto result = S_OK;
//
//    typedef std::pair <std::shared_ptr<wchar_t[]>, unsigned int> CsoData;
//    static std::unordered_map < std::wstring, CsoData> cache;
//
//
//    auto it = cache.find(filename);
//    if (it != cache.end())
//    {
//        data = it->second.first;
//        size = it->second.second;
//        return S_OK;
//    }
//
//    std::wifstream file(filename);
//
//    file.seekg(0, std::ios_base::end);
//    size = file.tellg();
//    file.seekg(0);
//    std::shared_ptr<wchar_t[]> csodata = std::make_shared<wchar_t[]>(size);
//    file.read(csodata.get(), size);
//    file.close();
//    data.reset();
//    data = csodata;
//
//    CsoData fragment = std::make_pair(data, size);
//    cache.emplace(filename, fragment);
//
//    return result;
//}




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

HRESULT ResourceManager::CreatePSFromFile(D3D::DevicePtr& device, const std::string& ps_name, D3D::PSPtr& ps)
{
	static std::map<std::string, D3D::PSPtr> cache;

	// search another fragment
	auto it = cache.find(ps_name);
	if (it != cache.end())
	{
		ps = it->second;

		return S_OK;
	}

	// if another fragment doesn't exist, open csoFile.
	// 頂点シェーダーのHLSLファイルの入力
	FILE* fp = nullptr;
	fopen_s(&fp, ps_name.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	long csoSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	std::unique_ptr<unsigned char[]> csoData;
	csoData = std::make_unique<unsigned char[]>(csoSize);
	fread(csoData.get(), csoSize, 1, fp);
	fclose(fp);



	HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, ps.GetAddressOf());
	if (FAILED(hr))
	{
		Log::Error("[RESOURCE MANAGER] Couldn't Create Pixel Shader. (%s)", ps_name.c_str());
	}


	// insert new element
	cache.insert(std::make_pair(ps_name, ps));

	return hr;
}


// @brief Bind the PATH that reffererFilename has TO targetFilename
// @param wchar_t*        newFilename       : Output;new filename
// @param const wchar_t*  reffererFilename  : it has PATH that we need
// @param const wchar_t*  targetFilename    : want to make this bind PATH
HRESULT ResourceManager::CreateFilenameToRefer(
	wchar_t(&newFilename)[256],
	const wchar_t* reffererFilename,
	const wchar_t* targetFilename,
	bool will_be_deleted)
{
	HRESULT hr = S_OK;
	const wchar_t delimiters[] = { L'\\', L'/' };

	wchar_t directory[256] = {};
	for (wchar_t delimiter : delimiters)
	{
		wchar_t* p = wcsrchr(const_cast<wchar_t*>(reffererFilename), delimiter);
		if (p)
		{
			memcpy_s(directory, 256, reffererFilename, (p - reffererFilename + 1) * sizeof(wchar_t));
			break;
		}
	}
	wchar_t filename[256];
	wcscpy_s(filename, targetFilename);

	if (will_be_deleted)
	{
		for (wchar_t delimiter : delimiters)
		{
			wchar_t* p = wcsrchr(filename, delimiter);
			if (p)
			{
				wcscpy_s(filename, p + 1);
				break;
			}
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
	D3D::SRVPtr& p_srv)
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

	ID3D11Texture2D* texture2d;
	hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
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

