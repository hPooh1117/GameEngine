#include "SceneTest.h"

#include <algorithm>

#include "./Component/MeshComponent.h"

#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/ComputeExecuter.h"
#include "./Engine/GameSystem.h"
#include "./Engine/LightController.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Settings.h"

#include "./Renderer/Blender.h"
#include "./Renderer/ComputedTexture.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/ResourceManager.h"
#include "./Renderer/Renderer.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Texture.h"

#include "./Component/MeshComponent.h"
#include "./Component/MoveRotationComponent.h"

#include "./Utilities/Util.h"


//----------------------------------------------------------------------------------------------------------------------------

SceneTest::SceneTest(SceneManager* p_manager, Graphics::GraphicsDevice* p_device) :
	Scene(p_manager, p_device),
	mbAutoSetParameter(true),
	mpPrefilter(std::make_unique<PrefilterForPBR>())
{
	mNextScene = SceneID::SCENE_A;
	mPreComputeState = 0;
	ENGINE.GetRenderer()->GetMeshRenderer()->SetSkybox(SkyboxTextureID::EFootprintCourt);

	int count = 0;
	Actor* pPaintedMetal = new Actor();
	pPaintedMetal->SetScale(5.0f, 5.0f, 5.0f);
	pPaintedMetal->SetPosition(Vector3(-17.5f, -5.0f, 0.0f));
	pPaintedMetal->AddComponent<MeshComponent>();
	pPaintedMetal->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	pPaintedMetal->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Base_Color.png", TextureConfig::EColorMap);
	pPaintedMetal->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Normal.png", TextureConfig::ENormalMap);
	pPaintedMetal->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Height.png", TextureConfig::EHeightMap);
	pPaintedMetal->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_1K_Metallic.png", TextureConfig::EMetallicMap);
	pPaintedMetal->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_1K_Roughness.png", TextureConfig::ERoughnessMap);
	pPaintedMetal->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_1K_AO.png", TextureConfig::EAOMap);
	ENGINE.GetActorManagerPtr()->AddActor(pPaintedMetal, count++);

	Actor* pWoodedPlank = new Actor();
	pWoodedPlank->SetScale(5.0f, 5.0f, 5.0f);
	pWoodedPlank->SetPosition(Vector3(-17.5f, 0.0f, 0.0f));
	pWoodedPlank->AddComponent<MeshComponent>();
	pWoodedPlank->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	pWoodedPlank->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_2K_Base_Color.png", TextureConfig::EColorMap);
	pWoodedPlank->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_2K_Normal.png", TextureConfig::ENormalMap);
	pWoodedPlank->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_2K_Height.png", TextureConfig::EHeightMap);
	pWoodedPlank->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Marble_08_2K_Metallic.png", TextureConfig::EMetallicMap);
	pWoodedPlank->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_1K_Roughness.png", TextureConfig::ERoughnessMap);
	pWoodedPlank->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_1K_AO.png", TextureConfig::ERoughnessMap);
	ENGINE.GetActorManagerPtr()->AddActor(pWoodedPlank, count++);

	Actor* mpCopperTiles = new Actor();
	mpCopperTiles->SetScale(5.0f, 5.0f, 5.0f);
	mpCopperTiles->SetPosition(Vector3(-17.5f, 5.0f, 0.0f));
	mpCopperTiles->AddComponent<MeshComponent>();
	mpCopperTiles->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	mpCopperTiles->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_2K_Base_Color.png", TextureConfig::EColorMap);
	mpCopperTiles->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_2K_Normal.png", TextureConfig::ENormalMap);
	mpCopperTiles->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_2K_Height.png", TextureConfig::EHeightMap);
	mpCopperTiles->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_Metallic.png",      TextureConfig::EMetallicMap);
	mpCopperTiles->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_1K_Roughness.png", TextureConfig::ERoughnessMap);
	mpCopperTiles->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_1K_AO.png", TextureConfig::EAOMap);
	ENGINE.GetActorManagerPtr()->AddActor(mpCopperTiles, count++);

	Actor* pBlackHerringBoneTile = new Actor();
	pBlackHerringBoneTile->SetScale(5.0f, 5.0f, 5.0f);
	pBlackHerringBoneTile->SetPosition(Vector3(-17.5f, 10.0f, 0.0f));
	pBlackHerringBoneTile->AddComponent<MeshComponent>();
	pBlackHerringBoneTile->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	pBlackHerringBoneTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_1K_Base_Color.png", TextureConfig::EColorMap);
	pBlackHerringBoneTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_1K_Normal.png", TextureConfig::ENormalMap);
	pBlackHerringBoneTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_1K_Height.png", TextureConfig::EHeightMap);
	pBlackHerringBoneTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Marble_08_2K_Metallic.png", TextureConfig::EMetallicMap);
	pBlackHerringBoneTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_1K_Roughness.png", TextureConfig::ERoughnessMap);
	pBlackHerringBoneTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_1K_AO.png", TextureConfig::EAOMap);
	ENGINE.GetActorManagerPtr()->AddActor(pBlackHerringBoneTile, count++);

	Actor* pBlurGreenHexagonalTile = new Actor();
	pBlurGreenHexagonalTile->SetScale(5.0f, 5.0f, 5.0f);
	pBlurGreenHexagonalTile->SetPosition(Vector3(-17.5f, 15.0f, 0.0f));
	pBlurGreenHexagonalTile->AddComponent<MeshComponent>();
	pBlurGreenHexagonalTile->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	pBlurGreenHexagonalTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_1K_Base_Color.png", TextureConfig::EColorMap);
	pBlurGreenHexagonalTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_1K_Normal.png", TextureConfig::ENormalMap);
	pBlurGreenHexagonalTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_1K_Height.png", TextureConfig::EHeightMap);
	pBlurGreenHexagonalTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Marble_08_2K_Metallic.png", TextureConfig::EMetallicMap);
	pBlurGreenHexagonalTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_1K_Roughness.png", TextureConfig::ERoughnessMap);
	pBlurGreenHexagonalTile->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_1K_AO.png", TextureConfig::EAOMap);

	ENGINE.GetActorManagerPtr()->AddActor(pBlurGreenHexagonalTile, count++);



	for (auto i = 0; i < 25; ++i)
	{
		Actor* pPBRSphere = new Actor();
		pPBRSphere->SetScale(5.0f, 5.0f, 5.0f);
		pPBRSphere->SetPosition(Vector3((i % 5) * 5.0f - 10.0f, i / 5 * 5.0f - 5.0f, 0.0f));
		pPBRSphere->AddComponent<MeshComponent>();
		pPBRSphere->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
		pPBRSphere->GetComponent<MeshComponent>()->SetBRDFFactors(0.2f * (i / 5), 0.2f * (i % 5));
		pPBRSphere->GetComponent<MeshComponent>()->SetMaterialColor(Vector4(0.5f, 0.0f, 0.0f, 1.0f));
		ENGINE.GetActorManagerPtr()->AddActor(pPBRSphere, count++);
	}

	Actor* pPBR = new Actor();
	pPBR->SetScale(5.0f, 5.0f, 5.0f);
	pPBR->SetPosition(Vector3(17.5f, 10.0f, 0.0f));
	pPBR->AddComponent<MeshComponent>();
	pPBR->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EForwardPBR, nullptr, FbxType::EDefault);
	//pPBR->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_3K_Base_Color.png", 0);
	pPBR->GetComponent<MeshComponent>()->SetMaterialColor(Vector4(0.8f, 0.5f, 0.1f, 1.0f));
	ENGINE.GetActorManagerPtr()->AddActor(pPBR, count++);

	Actor* pModel = new Actor();
	pModel->SetScale(0.5f, 0.5f, 0.5f);
	pModel->SetPosition(Vector3(17.5f, -5.0f, 0.0f));
	pModel->AddComponent<MeshComponent>();
	pModel->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::EForwardPBR, L"./Data/Models/ShaderBall.fbx", FbxType::EDefault);
	pModel->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/metal-tiles-03/Metal_tiles_03_1K_Base_Color.png", TextureConfig::EColorMap);
	pModel->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/metal-tiles-03/Metal_tiles_03_1K_Normal.png", TextureConfig::ENormalMap);
	pModel->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/metal-tiles-03/Metal_tiles_03_1K_Height.png", TextureConfig::EHeightMap);
	pModel->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/metal-tiles-03/Metal_tiles_03_1K_Metallic.png", TextureConfig::EMetallicMap);
	pModel->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/metal-tiles-03/Metal_tiles_03_1K_Roughness.png", TextureConfig::ERoughnessMap);
	pModel->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/metal-tiles-03/Metal_tiles_03_1K_AO.png", TextureConfig::EAOMap);

	ENGINE.GetActorManagerPtr()->AddActor(pModel, count++);



	ENGINE.GetLightPtr()->Init(0, 0);
	ENGINE.GetLightPtr()->SetLightColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

	ENGINE.GetCameraPtr()->SetTarget(pPaintedMetal);
	ENGINE.GetCameraPtr()->SetPositionOfMoveableCamera(Vector3(0.0f, 10.0f, -60.0f));


	Settings::Renderer renderSettings = {
	false,   // shadow
	false,  // ssao
	false,   // deffered
	false   // cubemap
	};
	ENGINE.SetRendererSettings(renderSettings);


	Actor* temp = new Actor();
	temp->AddComponent<MeshComponent>();
	temp->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::ECubeMapEnv, nullptr, FbxType::EDefault);
	temp->SetPosition(Vector3(0.0, -3.0f, -10.0f));
	temp->SetScale(4.0f, 4.0f, 4.0f);
	ENGINE.GetActorManagerPtr()->AddActor(temp, count++);

	mpPrefilter->Initialize(p_device);
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
		ENGINE.GetActorManagerPtr()->GetActor(EPBRSphereParam)->GetComponent<MeshComponent>()->SetBRDFFactors(0.0f, rough);
		rough += displacement * 0.016f;
		if (rough >= 1.0f || rough <= 0.0f) displacement *= -1;
	}

	ENGINE.GetActorManagerPtr()->Update(elapsed_time);
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneTest::PreCompute(Graphics::GraphicsDevice* p_graphics)
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneTest::Render(Graphics::GraphicsDevice* p_device, float elapsed_time)
{
	mpPrefilter->PreCompute(p_device);
	mpPrefilter->SetModifiedTextures(p_device);
}

void SceneTest::RenderUI()
{
	auto* ui = ENGINE.GetRenderer()->GetUIRenderer();
	ui->SetNextWindowSettings(Vector2(0, SCREEN_HEIGHT - 200), Vector2(300, 200));
	ui->SetNextUIConfig(false);

	ui->BeginRenderingNewWindow("PBR");
	ImGui::Checkbox("Auto Mode", &mbAutoSetParameter);
	ENGINE.GetActorManagerPtr()->GetActor(EPBRSphereParam)->GetComponent<MeshComponent>()->RenderUI();


	ui->FinishRenderingWindow();

}

//----------------------------------------------------------------------------------------------------------------------------

SceneTest::~SceneTest()
{
}
