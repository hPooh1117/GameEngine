#include "ComputeExecuter.h"

#include "./Renderer/Shader.h"

#include "./Utilities/Log.h"

const std::wstring ComputeExecuter::SHADER_FILE_PATH = L"./Src/Shaders/";

ComputeExecuter::ComputeExecuter()
{
}

ComputeExecuter::~ComputeExecuter()
{
}

bool ComputeExecuter::Initialize(D3D::DevicePtr& p_device)
{
	AddComputeShader(p_device, ComputeID::ESample, L"ComputeSample.hlsl", "CSmain");



	return true;
}

bool ComputeExecuter::CreateStructuredBufferOnGPU(D3D::DevicePtr& p_device, unsigned int id, UINT element_size, UINT count, void* pInitData, bool isForOutput)
{
	auto result = S_OK;

	D3D11_BUFFER_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(bufDesc));
	bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufDesc.ByteWidth = element_size * count;
	bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufDesc.StructureByteStride = element_size;

	
	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = pInitData;
		if (!isForOutput) result = p_device->CreateBuffer(&bufDesc, &data, mpStructuredBuffers[id].GetAddressOf());
		else              result = p_device->CreateBuffer(&bufDesc, &data, mpOutputBuffers[id].GetAddressOf());
	}
	else
	{
		if (!isForOutput) result = p_device->CreateBuffer(&bufDesc, NULL, mpStructuredBuffers[id].GetAddressOf());
		else              result = p_device->CreateBuffer(&bufDesc, NULL, mpOutputBuffers[id].GetAddressOf());
	}
	if (FAILED(result))
	{
		Log::Error("[COMPUTE EXECUTER] Couldn't create structured buffer. (id : %d)", id);
		return false;
	}

	return true;
}

bool ComputeExecuter::CreateBufferSRV(D3D::DevicePtr& p_device, unsigned int id)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	mpStructuredBuffers[id]->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// そのままのバッファ処理
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
	{
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// 構造体バッファ処理
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return false;
		}
	}


	auto result = p_device->CreateShaderResourceView(mpStructuredBuffers[id].Get(), &desc, mpSrcSRVs[id].GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[COMPUTE EXECUTER] Couldn't create srv (id : %d)", id);
		return false;
	}
	return true;
}

bool ComputeExecuter::CreateBufferUAV(D3D::DevicePtr& p_device, unsigned int id)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	mpOutputBuffers[id]->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// そのままのバッファ処理
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else
	{
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// 構造体バッファ処理
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return false;
		}
	}


	auto result = p_device->CreateUnorderedAccessView(mpOutputBuffers[id].Get(), &desc, mpDestUAVs[id].GetAddressOf());
	if (FAILED(result))
	{
		Log::Error("[COMPUTE EXECUTER] Couldn't create uav (id : %d)", id);
		return false;
	}

	return true;
}

void ComputeExecuter::Execute(D3D::DeviceContextPtr& p_imm_context, unsigned int id)
{
	if (id < 0 || id >= ComputeID::COMPUTE_ID_MAX) return;

	ID3D11UnorderedAccessView* pUAVNull = nullptr;
	ID3D11ShaderResourceView*  pSRVNull = nullptr;

	mpCSTable[id]->ActivateCSShader(p_imm_context);
	p_imm_context->CSSetShaderResources(0, 1, mpSrcSRVs[id].GetAddressOf());
	p_imm_context->CSSetUnorderedAccessViews(0, 1, mpDestUAVs[id].GetAddressOf(), NULL);

	p_imm_context->Dispatch(16, 16, 1);

	p_imm_context->CSSetUnorderedAccessViews(0, 1, &pUAVNull, NULL);
	p_imm_context->CSSetShaderResources(0, 1, &pSRVNull);
	mpCSTable[id]->DeactivateCSShader(p_imm_context);
}

void ComputeExecuter::AddComputeShader(D3D::DevicePtr& p_device, unsigned int id, const wchar_t* hlsl_file, const char* cs_entry)
{
	std::wstring csFilename = SHADER_FILE_PATH + std::wstring(hlsl_file);
	mpCSTable[id] = std::make_unique<Shader>();
	mpCSTable.back()->CreateComputeShader(p_device, csFilename, cs_entry);
}
