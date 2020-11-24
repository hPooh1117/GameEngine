#include "SceneTest.h"

#include <algorithm>

#include "./Component/MeshComponent.h"

#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/GameSystem.h"
#include "./Engine/LightController.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Settings.h"
#include "./Engine/ComputeExecuter.h"

#include "./Renderer/Blender.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Shader.h"
#include "./Renderer/ResourceManager.h"
#include "./Renderer/ComputedTexture.h"
#include "./Renderer/Texture.h"

#include "./Component/MoveRotationComponent.h"
#include "./Component/MeshComponent.h"

#include "./Utilities/Util.h"


//----------------------------------------------------------------------------------------------------------------------------

SceneTest::SceneTest(SceneManager* p_manager, Microsoft::WRL::ComPtr<ID3D11Device>& p_device) :
	Scene(p_manager, p_device),
	mbAutoSetParameter(true)
{
	mNextScene = SceneID::SCENE_A;

	int count = 0;
	mpPaintedMetal = Actor::Initialize(count++);
	mpPaintedMetal->SetScale(5.0f, 5.0f, 5.0f);
	mpPaintedMetal->AddComponent<NewMeshComponent>();
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Base_Color.png", TextureConfig::EColorMap);
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Normal.png", TextureConfig::ENormalMap);
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Height.png", TextureConfig::EHeightMap);
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_1K_Metallic.png", TextureConfig::EMetallicMap);
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_1K_Roughness.png", TextureConfig::ERoughnessMap);
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_1K_AO.png", TextureConfig::EAOMap);
	ENGINE.GetActorManagerPtr()->AddActor(mpPaintedMetal);

	mpWoodedPlank = Actor::Initialize(count++);
	mpWoodedPlank->SetScale(5.0f, 5.0f, 5.0f);
	mpWoodedPlank->SetPosition(Vector3(5.0f, 0.0f, 0.0f));
	mpWoodedPlank->AddComponent<NewMeshComponent>();
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_2K_Base_Color.png", TextureConfig::EColorMap);
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_2K_Normal.png", TextureConfig::ENormalMap);
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_2K_Height.png", TextureConfig::EHeightMap);
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Marble_08_2K_Metallic.png", TextureConfig::EMetallicMap);
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_1K_Roughness.png", TextureConfig::ERoughnessMap);
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_1K_AO.png", TextureConfig::ERoughnessMap);
	ENGINE.GetActorManagerPtr()->AddActor(mpWoodedPlank);

	mpCopperTiles = Actor::Initialize(count++);
	mpCopperTiles->SetScale(5.0f, 5.0f, 5.0f);
	mpCopperTiles->SetPosition(Vector3(10.0f, 0.0f, 0.0f));
	mpCopperTiles->AddComponent<NewMeshComponent>();
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_2K_Base_Color.png", TextureConfig::EColorMap);
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_2K_Normal.png", TextureConfig::ENormalMap);
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_2K_Height.png", TextureConfig::EHeightMap);
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_Metallic.png",      TextureConfig::EMetallicMap);
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_1K_Roughness.png", TextureConfig::ERoughnessMap);
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_1K_AO.png", TextureConfig::EAOMap);
	ENGINE.GetActorManagerPtr()->AddActor(mpCopperTiles);

	mpBlackHerringBoneTile = Actor::Initialize(count++);
	mpBlackHerringBoneTile->SetScale(5.0f, 5.0f, 5.0f);
	mpBlackHerringBoneTile->SetPosition(Vector3(-5.0f, 0.0f, 0.0f));
	mpBlackHerringBoneTile->AddComponent<NewMeshComponent>();
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_1K_Base_Color.png", TextureConfig::EColorMap);
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_1K_Normal.png", TextureConfig::ENormalMap);
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_1K_Height.png", TextureConfig::EHeightMap);
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Marble_08_2K_Metallic.png", TextureConfig::EMetallicMap);
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_1K_Roughness.png", TextureConfig::ERoughnessMap);
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_1K_AO.png", TextureConfig::EAOMap);
	ENGINE.GetActorManagerPtr()->AddActor(mpBlackHerringBoneTile);

	mpBlurGreenHexagonalTile = Actor::Initialize(count++);
	mpBlurGreenHexagonalTile->SetScale(5.0f, 5.0f, 5.0f);
	mpBlurGreenHexagonalTile->SetPosition(Vector3(-10.0f, 0.0f, 0.0f));
	mpBlurGreenHexagonalTile->AddComponent<NewMeshComponent>();
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_1K_Base_Color.png", TextureConfig::EColorMap);
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_1K_Normal.png", TextureConfig::ENormalMap);
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_1K_Height.png", TextureConfig::EHeightMap);
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Marble_08_2K_Metallic.png", TextureConfig::EMetallicMap);
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_1K_Roughness.png", TextureConfig::ERoughnessMap);
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_1K_AO.png", TextureConfig::EAOMap);

	ENGINE.GetActorManagerPtr()->AddActor(mpBlurGreenHexagonalTile);



	for (auto i = 0; i < 25; ++i)
	{
		mpActors[i] = Actor::Initialize(count++);
		mpActors[i]->SetScale(5.0f, 5.0f, 5.0f);
		mpActors[i]->SetPosition(Vector3((i % 5) * 5.0f - 10.0f, i / 5 * 5.0f + 5.0f, 0.0f));
		mpActors[i]->AddComponent<NewMeshComponent>();
		mpActors[i]->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
		mpActors[i]->GetComponent<NewMeshComponent>()->SetBRDFFactors(0.2f * (i / 5), 0.2f * (i % 5));
		mpActors[i]->GetComponent<NewMeshComponent>()->SetMaterialColor(Vector4(0.5f, 0.0f, 0.0f, 1.0f));
		ENGINE.GetActorManagerPtr()->AddActor(mpActors[i]);
	}

	mpPBR = Actor::Initialize(count++);
	mpPBR->SetScale(5.0f, 5.0f, 5.0f);
	mpPBR->SetPosition(Vector3(0.0f, 0.0f, -6.0f));
	mpPBR->AddComponent<NewMeshComponent>();
	mpPBR->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	//mpPBR->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_3K_Base_Color.png", 0);
	mpPBR->GetComponent<NewMeshComponent>()->SetMaterialColor(Vector4(0.8f, 0.5f, 0.1f, 1.0f));
	ENGINE.GetActorManagerPtr()->AddActor(mpPBR);

	//std::shared_ptr<Actor> pPlane = Actor::Create(count++);
	//pPlane->SetScale(10.0f, 1.0f, 10.0f);
	//pPlane->SetPosition(Vector3(0.0f, 10.0f, -3.0f));
	//pPlane->AddComponent<NewMeshComponent>();
	//pPlane->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_Plane, ShaderID::ECubemapConvolution, nullptr);
	//pPlane->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg", TextureConfig::EColorMap);
	//ENGINE.GetActorManagerPtr()->AddActor(pPlane);


	ENGINE.GetLightPtr()->Init(0, 0);
	ENGINE.GetLightPtr()->SetLightColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

	ENGINE.GetCameraPtr()->SetTarget(mpPaintedMetal);


	Settings::Renderer renderSettings = {
	false,   // shadow
	false,  // ssao
	false,   // deffered
	false   // cubemap
	};
	ENGINE.SetRendererSettings(renderSettings);

	//ID3D11ShaderResourceView* srv = nullptr;
	//D3D11_TEXTURE2D_DESC desc = {};
	//ResourceManager::LoadTexFile(p_device, L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_3K_Normal.png", &srv, &desc);

	mpIrradianceTex = std::make_unique<ComputedTexture>();
	mpIrradianceTex->CreateSampler(p_device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
	mpIrradianceTex->CreateTextureCube(p_device, 32, 32, DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
	mpIrradianceTex->CreateTextureUAV(p_device, 0);
	mpIrradianceTex->CreateShader(p_device, L"./Src/Shaders/CS_PreComputingIrradiance.hlsl", "CSmain");

	mpSpecularMapTex = std::make_unique<ComputedTexture>();
	mpSpecularMapTex->CreateSampler(p_device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
	mpSpecularMapTex->CreateTextureCube(p_device, 1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpSpecularMapTex->CreateShader(p_device, L"Src/Shaders/CS_PreFilteringForSpecular.hlsl", "CSmain");

	mpSkyTex = std::make_unique<NewTexture>();
	mpSkyTex->Load(p_device, L"./Data/Images/Environment/Footprint_Court/Footprint_Court_2k.hdr");

	mpEnvironmentTex = std::make_unique<ComputedTexture>();
	mpEnvironmentTex->CreateSampler(p_device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
	mpEnvironmentTex->CreateTextureCube(p_device, 1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mpEnvironmentTex->CreateTextureUAV(p_device, 0);
	mpEnvironmentTex->CreateShader(p_device, L"./Src/Shaders/CS_Equirectangular2Cube.hlsl", "CSmain");

	mpSpecularBRDF_LUT = std::make_unique<ComputedTexture>();
	mpSpecularBRDF_LUT->CreateSampler(p_device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
	mpSpecularBRDF_LUT->CreateTexture(p_device, 256, 256, DXGI_FORMAT_R16G16_FLOAT, 1);
	mpSpecularBRDF_LUT->CreateTextureUAV(p_device, 0);
	mpSpecularBRDF_LUT->CreateShader(p_device, L"./Src/Shaders/CS_SpecularBRDF.hlsl", "CSmain");

	std::shared_ptr<Actor> temp = Actor::Initialize(count++);
	temp->AddComponent<NewMeshComponent>();
	temp->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::ECubeMapEnv, nullptr, FbxType::EDefault);
	temp->SetPosition(Vector3(0.0, -3.0f, -10.0f));
	temp->SetScale(4.0f, 4.0f, 4.0f);
	ENGINE.GetActorManagerPtr()->AddActor(temp);


}

//----------------------------------------------------------------------------------------------------------------------------

void SceneTest::InitializeScene()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneTest::Update(float elapsed_time)
{
	if (mbAutoSetParameter)
	{
		static float rough = 0.0f;
		static float displacement = 0.1f;
		mpPBR->GetComponent<NewMeshComponent>()->SetBRDFFactors(0.0f, rough);
		rough += displacement * 0.016f;
		if (rough >= 1.0f || rough <= 0.0f) displacement *= -1;
	}

	ENGINE.GetActorManagerPtr()->Update(elapsed_time);
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneTest::PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	//static bool flag = false;
	//if (!flag)
	//{
	//	D3D::DevicePtr pDevice = p_graphics->GetDevicePtr();
	//	D3D::DeviceContextPtr pImmContext = p_graphics->GetImmContextPtr();

	//	mpEnvironmentTex->Compute(pImmContext, mpSkyTex->GetSRV(), 6);

	//	pImmContext->GenerateMips(mpEnvironmentTex->GetSRV().Get());

	//	const UINT MipLevels = mpSpecularMapTex->GetMipLevels();
	//	// ミップマップレベル0の環境マップをコピー
	//	for (int arraySlice = 0; arraySlice < 6; ++arraySlice)
	//	{
	//		const UINT subresourceIndex = D3D11CalcSubresource(0, arraySlice, MipLevels);
	//		pImmContext->CopySubresourceRegion(mpSpecularMapTex->GetTexture2D().Get(), subresourceIndex, 0, 0, 0, mpEnvironmentTex->GetTexture2D().Get(), subresourceIndex, nullptr);
	//	}

	//	// 他のミップマップレベルの環境マップをプリフィルタリング
	//	const float deltaRoughness = 1.0f / std::max<float>(static_cast<float>(MipLevels - 1), 1.0f);

	//	for (UINT level = 1, size = 512; level < MipLevels; ++level, size /= 2)
	//	{
	//		const UINT numGroups = std::max<UINT>(1, size / 32);
	//		mpSpecularMapTex->CreateTextureUAV(pDevice, level);
	//		mpSpecularMapTex->SetCBuffer(pImmContext, level * deltaRoughness);
	//		mpSpecularMapTex->Compute(pImmContext, mpEnvironmentTex->GetSRV(), numGroups, numGroups, 6);
	//	}

	//	mpIrradianceTex->Compute(pImmContext, mpSpecularMapTex->GetSRV(), 6);
	//	//mpIrradianceTex->Compute(pImmContext, mpEnvironmentTex->GetSRV(), 6);
	//	mpSpecularBRDF_LUT->Compute(pImmContext, 1);

	//	flag = true;
	//}
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneTest::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	D3D::DevicePtr pDevice = p_graphics->GetDevicePtr();
	D3D::DeviceContextPtr pImmContext = p_graphics->GetImmContextPtr();

	static int state = 0;
	switch(state++)
	{
	case 0:
		mpEnvironmentTex->Compute(pImmContext, mpSkyTex->GetSRV(), 6);
		pImmContext->GenerateMips(mpEnvironmentTex->GetSRV().Get());
		Log::Info("Pre-Computed Equirectangular To Cube. (state : %d)", state);
		break;
	case 1:
	{
		const UINT MipLevels = mpSpecularMapTex->GetMipLevels();
		// ミップマップレベル0の環境マップをコピー
		for (int arraySlice = 0; arraySlice < 6; ++arraySlice)
		{
			const UINT subresourceIndex = D3D11CalcSubresource(0, arraySlice, MipLevels);
			pImmContext->CopySubresourceRegion(mpSpecularMapTex->GetTexture2D().Get(), subresourceIndex, 0, 0, 0, mpEnvironmentTex->GetTexture2D().Get(), subresourceIndex, nullptr);
		}

		// 他のミップマップレベルの環境マップをプリフィルタリング
		const float deltaRoughness = 1.0f / std::max<float>(static_cast<float>(MipLevels - 1), 1.0f);

		for (UINT level = 1, size = 512; level < MipLevels; ++level, size /= 2)
		{
			const UINT numGroups = std::max<UINT>(1, size / 32);
			mpSpecularMapTex->CreateTextureUAV(pDevice, level);
			mpSpecularMapTex->SetCBuffer(pImmContext, level * deltaRoughness);
			mpSpecularMapTex->Compute(pImmContext, mpEnvironmentTex->GetSRV(), numGroups, numGroups, 6);
		}
		Log::Info("Pre-Computed SpecularIBL Filter. (state : %d)", state);
		break;
	}
	case 2:
		mpIrradianceTex->Compute(pImmContext, mpSpecularMapTex->GetSRV(), 6);
		Log::Info("Pre-Computed Diffuse Irardiance. (state : %d)", state);
		break;
	case 3:
	//	mpIrradianceTex->Compute(pImmContext, 6);
	//	Log::Info("Pre-Computed Diffuse Irardiance(2nd). (state : %d)", state);
	//	break;
	//case 4:
		mpSpecularBRDF_LUT->Compute(pImmContext, 1);
		Log::Info("Pre-Computed Specular Look-Up Texture. (state : %d)", state);
		break;
	}

	//D3D::DeviceContextPtr pImmContext = p_graphics->GetImmContextPtr();

	mpIrradianceTex->Set(pImmContext, 10);
	mpSpecularMapTex->Set(pImmContext, 11);
	mpSpecularBRDF_LUT->Set(pImmContext, 12);
	mpEnvironmentTex->Set(pImmContext, 13);

	ENGINE.GetUIRenderer()->SetNextWindowSettings(Vector2(0, SCREEN_HEIGHT - 200), Vector2(300, 200));
	ENGINE.GetUIRenderer()->BeginRenderingNewWindow("PBR");
	ImGui::Checkbox("Auto Mode", &mbAutoSetParameter);
	mpPBR->GetComponent<NewMeshComponent>()->RenderUI();
	ImGui::Image((void*)mpSpecularBRDF_LUT->GetSRV().Get(), ImVec2(320, 320));
	ENGINE.GetUIRenderer()->FinishRenderingWindow();
}

//----------------------------------------------------------------------------------------------------------------------------

SceneTest::~SceneTest()
{
}
