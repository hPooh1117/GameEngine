#pragma once
#include <array>
#include <memory>
#include <string>

#include "./Renderer/D3D_Helper.h"

#include "./Utilities/Log.h"

class Shader;

class ComputeExecuter
{
public:
	enum ComputeID
	{
		ESample,



		COMPUTE_ID_MAX,
	};

private:
	static const std::wstring SHADER_FILE_PATH;


	std::array<D3D::BufferPtr, COMPUTE_ID_MAX> mpStructuredBuffers;
	std::array<D3D::BufferPtr, COMPUTE_ID_MAX> mpOutputBuffers;
	std::array<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>, COMPUTE_ID_MAX> mpDestUAVs;
	std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, COMPUTE_ID_MAX>  mpSrcSRVs;

	std::array<std::unique_ptr<Shader>, COMPUTE_ID_MAX> mpCSTable;

public:
	ComputeExecuter();
	~ComputeExecuter();

	bool Initialize(D3D::DevicePtr& p_device);
	bool CreateStructuredBufferOnGPU(D3D::DevicePtr& p_device, unsigned int id, UINT element_size, UINT count, void* pInitData, bool isForOutput = false);
	bool CreateBufferSRV(D3D::DevicePtr& p_device, unsigned int id);
	bool CreateBufferUAV(D3D::DevicePtr& p_device, unsigned int id);
	void Execute(D3D::DeviceContextPtr& p_imm_context, unsigned int id);

	template<class T>
	void FetchDataComputed(D3D::DevicePtr& p_device, D3D::DeviceContextPtr& p_imm_context, unsigned int id, unsigned int count, T* output);

private:
	void AddComputeShader(D3D::DevicePtr& p_device, unsigned int id, const wchar_t* hlsl_file, const char* cs_entry);

};


template<class T>
void ComputeExecuter::FetchDataComputed(
	D3D::DevicePtr& p_device, 
	D3D::DeviceContextPtr& p_imm_context,
	unsigned int id,
	unsigned int count,
	T* output)
{
	auto result = S_OK;


	D3D::BufferPtr buf = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	mpOutputBuffers[id]->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	result = p_device->CreateBuffer(&desc, NULL, buf.GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[COMPUTE EXECUTER] Couldn't create buffer to read result of compute shader. (id : %d)", id);
		return;
	}

	p_imm_context->CopyResource(buf.Get(), mpOutputBuffers[id].Get());

	D3D11_MAPPED_SUBRESOURCE mappedResouce;
	result = p_imm_context->Map(buf.Get(), 0, D3D11_MAP_READ, 0, &mappedResouce);
	if (FAILED(result))
	{
		Log::Error("[[COMPUTE EXECUTER] Couldn't read computed data.");
	}
	

	UINT size = sizeof(T);
	memcpy(output, mappedResouce.pData, size * count);

	p_imm_context->Unmap(buf.Get(), 0);
}
