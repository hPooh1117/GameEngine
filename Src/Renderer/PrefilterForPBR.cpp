#include "PrefilterForPBR.h"

bool PrefilterForPBR::Initialize(Graphics::GraphicsDevice* p_device)
{
	auto& Device = p_device->GetDevicePtr();
	
	mpShaders[EPreComputeIrradiance]->CreateComputeShader(Device, L"./Src/Shaders/CS_PreComputingIrradiance.hlsl", "CSmain");
	mpShaders[EPreComputeSpecular]->CreateComputeShader(Device, L"Src/Shaders/CS_PreFilteringForSpecular.hlsl", "CSmain");
	mpShaders[EEquirectangular2Cube]->CreateComputeShader(Device, L"./Src/Shaders/CS_Equirectangular2Cube.hlsl", "CSmain");
	mpShaders[ESpecularBRDF]->CreateComputeShader(Device, L"./Src/Shaders/CS_SpecularBRDF.hlsl", "CSmain");

	mpIrradianceTex->CreateTextureCube(Device, 32, 32, DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
	mpIrradianceTex->CreateTextureUAV(Device, 0);
	//mpIrradianceTex->CreateShader(Device, L"./Src/Shaders/CS_PreComputingIrradiance.hlsl", "CSmain");

	mpSpecularMapTex->CreateTextureCube(Device, 1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
	//mpSpecularMapTex->CreateShader(Device, L"Src/Shaders/CS_PreFilteringForSpecular.hlsl", "CSmain");


	mpEnvironmentTex->CreateTextureCube(Device, 1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpEnvironmentTex->CreateTextureUAV(Device, 0);
	//mpEnvironmentTex->CreateShader(Device, L"./Src/Shaders/CS_Equirectangular2Cube.hlsl", "CSmain");

	mpSpecularBRDF_LUT->CreateTexture(Device, 256, 256, DXGI_FORMAT_R16G16_FLOAT, 1);
	mpSpecularBRDF_LUT->CreateTextureUAV(Device, 0);
	//mpSpecularBRDF_LUT->CreateShader(Device, L"./Src/Shaders/CS_SpecularBRDF.hlsl", "CSmain");


	mpSkyTex->Load(Device, L"./Data/Images/Environment/Footprint_Court/Footprint_Court_2k.hdr");

    return true;
}

void PrefilterForPBR::PreCompute(Graphics::GraphicsDevice* p_device)
{
	auto& pDevice = p_device->GetDevicePtr();
	auto& pImmContext = p_device->GetImmContextPtr();

	switch (mPreComputeState++)
	{
	case 0:
		p_device->SetSamplers(Graphics::SS_LINEAR_WRAP, 0);
		mpShaders[EEquirectangular2Cube]->ActivateCSShader(pImmContext);
		mpEnvironmentTex->Compute(p_device, mpSkyTex->GetSRV(), 6);
		pImmContext->GenerateMips(mpEnvironmentTex->GetSRV().Get());
		Log::Info("Pre-Computed Equirectangular To Cube. (state : %d)", mPreComputeState);
		break;
	case 1:
	{
		p_device->SetSamplers(Graphics::SS_LINEAR_WRAP, 0);

		const UINT MipLevels = mpSpecularMapTex->GetMipLevels();
		// ミップマップレベル0の環境マップをコピー
		for (int arraySlice = 0; arraySlice < 6; ++arraySlice)
		{
			const UINT subresourceIndex = D3D11CalcSubresource(0, arraySlice, MipLevels);
			pImmContext->CopySubresourceRegion(mpSpecularMapTex->GetTexture2D().Get(), subresourceIndex, 0, 0, 0, mpEnvironmentTex->GetTexture2D().Get(), subresourceIndex, nullptr);
		}

		// 他のミップマップレベルの環境マップをプリフィルタリング
		const float deltaRoughness = 1.0f / std::max<float>(static_cast<float>(MipLevels - 1), 1.0f);

		mpShaders[EPreComputeSpecular]->ActivateCSShader(pImmContext);

		for (UINT level = 1, size = 512; level < MipLevels; ++level, size /= 2)
		{
			const UINT numGroups = std::max<UINT>(1, size / 32);
			mpSpecularMapTex->CreateTextureUAV(pDevice, level);
			mpSpecularMapTex->SetCBuffer(pImmContext, level * deltaRoughness);
			mpSpecularMapTex->Compute(p_device, mpEnvironmentTex->GetSRV(), numGroups, numGroups, 6);
		}
		Log::Info("Pre-Computed SpecularIBL Filter. (state : %d)", mPreComputeState);
		break;
	}
	case 2:
		p_device->SetSamplers(Graphics::SS_LINEAR_WRAP, 0);
		mpShaders[EPreComputeIrradiance]->ActivateCSShader(pImmContext);
		mpIrradianceTex->Compute(p_device, mpSpecularMapTex->GetSRV(), 6);
		Log::Info("Pre-Computed Diffuse Irardiance. (state : %d)", mPreComputeState);
		break;
	case 3:
		p_device->SetSamplers(Graphics::SS_LINEAR_CLAMP, 0);
		mpShaders[ESpecularBRDF]->ActivateCSShader(pImmContext);
		mpSpecularBRDF_LUT->Compute(p_device, 1);
		Log::Info("Pre-Computed Specular Look-Up Texture. (state : %d)", mPreComputeState);
		break;
	default:
		break;
	}

	//D3D::DeviceContextPtr pImmContext = p_device->GetImmContextPtr();


}

void PrefilterForPBR::SetModifiedTextures(Graphics::GraphicsDevice* p_device)
{
	auto& pImmContext = p_device->GetImmContextPtr();
	mpIrradianceTex->Set(pImmContext, 10);
	mpSpecularMapTex->Set(pImmContext, 11);
	mpSpecularBRDF_LUT->Set(pImmContext, 12);
	mpEnvironmentTex->Set(pImmContext, 13);
}
