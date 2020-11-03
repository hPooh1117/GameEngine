#include "SceneTest.h"

#include "./Component/MeshComponent.h"

#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/GameSystem.h"
#include "./Engine/LightController.h"
#include "./Engine/UIRenderer.h"

#include "./Renderer/Blender.h"
#include "./Renderer/Skybox.h"

#include "./Component/MoveRotationComponent.h"
#include "./Component/MeshComponent.h"

SceneTest::SceneTest(SceneManager* p_manager, Microsoft::WRL::ComPtr<ID3D11Device>& p_device):Scene(p_manager, p_device)
{
	mNextScene = SceneID::SCENE_A;

	int count = 0;
	mpPaintedMetal = Actor::Initialize(count++);
	mpPaintedMetal->SetScale(5.0f, 5.0f, 5.0f);
	mpPaintedMetal->AddComponent<NewMeshComponent>();
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::EForwardPBR, nullptr, FbxType::EDefault);
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Base_Color.png", 0);
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg", 1);
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_3K_Metallic.png", 2);
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Roughness.png", 3);
	ENGINE.GetActorManagerPtr()->AddActor(mpPaintedMetal);

	mpWoodedPlank = Actor::Initialize(count++);
	mpWoodedPlank->SetScale(5.0f, 5.0f, 5.0f);
	mpWoodedPlank->SetPosition(Vector3(5.0f, 0.0f, 0.0f));
	mpWoodedPlank->AddComponent<NewMeshComponent>();
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::EForwardPBR, nullptr, FbxType::EDefault);
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_2K_Base_Color.png", 0);
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg", 1);
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Marble_08_2K_Metallic.png", 2);
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/wooden-planks-04/Wooden_planks_04_2K_Roughness.png", 3);
	ENGINE.GetActorManagerPtr()->AddActor(mpWoodedPlank);

	mpCopperTiles = Actor::Initialize(count++);
	mpCopperTiles->SetScale(5.0f, 5.0f, 5.0f);
	mpCopperTiles->SetPosition(Vector3(10.0f, 0.0f, 0.0f));
	mpCopperTiles->AddComponent<NewMeshComponent>();
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::EForwardPBR, nullptr, FbxType::EDefault);
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_2K_Base_Color.png", 0);
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg", 1);
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_Metallic.png", 2);
	mpCopperTiles->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/copper-tiles-01/Copper_tiles_01_2K_Roughness.png", 3);
	ENGINE.GetActorManagerPtr()->AddActor(mpCopperTiles);

	mpBlackHerringBoneTile = Actor::Initialize(count++);
	mpBlackHerringBoneTile->SetScale(5.0f, 5.0f, 5.0f);
	mpBlackHerringBoneTile->SetPosition(Vector3(-5.0f, 0.0f, 0.0f));
	mpBlackHerringBoneTile->AddComponent<NewMeshComponent>();
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::EForwardPBR, nullptr, FbxType::EDefault);
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_2K_Base_Color.png", 0);
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg", 1);
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/black-herringbone-tiles-01/Black_herringbone_tiles_01_2K_Roughness.png", 3);
	ENGINE.GetActorManagerPtr()->AddActor(mpBlackHerringBoneTile);

	mpBlurGreenHexagonalTile = Actor::Initialize(count++);
	mpBlurGreenHexagonalTile->SetScale(5.0f, 5.0f, 5.0f);
	mpBlurGreenHexagonalTile->SetPosition(Vector3(-10.0f, 0.0f, 0.0f));
	mpBlurGreenHexagonalTile->AddComponent<NewMeshComponent>();
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::EForwardPBR, nullptr, FbxType::EDefault);
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_3K_Base_Color.png", 0);
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg", 1);
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/blue-green-hexagonal-tiles-01/Blue_green_hexagonal_tiles_01_3K_Roughness.png", 3);
	ENGINE.GetActorManagerPtr()->AddActor(mpBlurGreenHexagonalTile);

	for (auto i = 0; i < 25; ++i)
	{
		mpActors[i] = Actor::Initialize(count++);
		mpActors[i]->SetScale(5.0f, 5.0f, 5.0f);
		mpActors[i]->SetPosition(Vector3((i % 5) * 5.0f - 10.0f, i / 5 * 5.0f + 5.0f, 0.0f));
		mpActors[i]->AddComponent<NewMeshComponent>();
		mpActors[i]->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::EForwardPBRSetting, nullptr, FbxType::EDefault);
		mpActors[i]->GetComponent<NewMeshComponent>()->SetBRDFFactors(0.2f * (i / 5), 0.2f * (i % 5));
		mpActors[i]->GetComponent<NewMeshComponent>()->SetMaterialColor(Vector4(0.7f, 0.32f, 0.16f, 1.0f));
		ENGINE.GetActorManagerPtr()->AddActor(mpActors[i]);
	}

	ENGINE.GetLightPtr()->Init(0, 0);
	ENGINE.GetCameraPtr()->SetTarget(mpPaintedMetal);

	ENGINE.SetIsDefferedRendering(false);
	ENGINE.SetSSAO(false);
	ENGINE.NotCastShadow();
}

void SceneTest::InitializeScene()
{
}

void SceneTest::Update(float elapsed_time)
{
	ENGINE.GetActorManagerPtr()->Update(elapsed_time);
}

void SceneTest::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	ENGINE.GetUIRenderer()->SetNextWindowSettings(Vector2(0, SCREEN_HEIGHT - 200), Vector2(300, 200));
	ENGINE.GetUIRenderer()->BeginRenderingNewWindow("PBR");
	mpPaintedMetal->GetComponent<NewMeshComponent>()->RenderUI();
	mpWoodedPlank->GetComponent<NewMeshComponent>()->RenderUI();
	mpCopperTiles->GetComponent<NewMeshComponent>()->RenderUI();
	mpBlackHerringBoneTile->GetComponent<NewMeshComponent>()->RenderUI();
	mpBlurGreenHexagonalTile->GetComponent<NewMeshComponent>()->RenderUI();

	ENGINE.GetUIRenderer()->FinishRenderingWindow();
}

SceneTest::~SceneTest()
{
}
