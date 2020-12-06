#include "SceneA.h"
//#include "SceneManager.h"

#include "./Application/Application.h"

#include "./Component/MeshComponent.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/GameSystem.h"
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Settings.h"

#include "./Component/MoveRotationComponent.h"

#include "./Renderer/Skybox.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/sprite.h"
#include "./Renderer/Blender.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/Shader.h"



using namespace DirectX;


SceneA::SceneA(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device) :Scene(manager, device)
{
	mNextScene = SceneID::SCENE_B;

	ENGINE.GetMeshRenderer()->SetSkybox(SkyboxTextureID::EWalkOfFame);

	int count = 0;
	// ----------------------------------------------------------------------------------------------
	// アクター・コンポーネント作成
	// ----------------------------------------------------------------------------------------------
	Actor* pSphere = new Actor();
	pSphere->SetScale(7.0f, 7.0f, 7.0f);
	pSphere->SetPosition(Vector3(7, 1, 0));
	pSphere->AddComponent<MoveRotationComponent>();
	pSphere->AddComponent<MeshComponent>();
	pSphere->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EEnvironmentMap, nullptr, FbxType::EDefault);
	pSphere->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/check.jpg", TextureConfig::EColorMap);
	pSphere->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/Environment/Walk_Of_Fame/Mans_Outside_2k.hdr", TextureConfig::EEnvironmentMap);
	ENGINE.GetActorManagerPtr()->AddActor(pSphere, count++);

	Actor* pEarth = new Actor();
	pEarth->SetScale(7.0f, 7.0f, 7.0f);
	pEarth->SetPosition(Vector3(0, 1, 0));
	pEarth->AddComponent<MoveRotationComponent>();
	pEarth->AddComponent<MeshComponent>();
	pEarth->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::ENormalMap, nullptr, FbxType::EDefault);
	pEarth->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/earthmap.jpg", TextureConfig::EColorMap);
	pEarth->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/earthnormal.jpg", TextureConfig::ENormalMap);
	pEarth->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/earthbump.jpg", TextureConfig::EHeightMap);
	ENGINE.GetActorManagerPtr()->AddActor(pEarth, count++);

	Actor* pToon = new Actor();
	pToon->SetScale(7, 7, 7);
	pToon->SetPosition(Vector3(-7, 1, 0));
	pToon->AddComponent<MoveRotationComponent>();
	pToon->AddComponent<MeshComponent>();
	pToon->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EToonGeo, nullptr, FbxType::EDefault);
	pToon->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/Test.png", TextureConfig::EColorMap);
	ENGINE.GetActorManagerPtr()->AddActor(pToon, count++);

	Actor* pFlat = new Actor();	
	pFlat->SetScale(1.5f, 1.5f, 1.5f);
	pFlat->SetPosition(Vector3(14, -2, 0));
	pFlat->AddComponent<MoveRotationComponent>();
	pFlat->AddComponent<MeshComponent>();
	pFlat->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_StaticMesh, ShaderID::EFlat, L"./Data/Models/OBJ/Mr.Incredible/Mr.Incredible.obj", FbxType::EMaya);
	//pFlat->GetComponent<MeshComponent>()->SetMaterialColor(Vector4(0.8f, 0.1f, 0.1f, 1.0f));
	//pFlat->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Base_Color.png", TextureConfig::EColorMap);
	//pFlat->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg",			 TextureConfig::EEnvironmentMap);
	//pFlat->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_3K_Metallic.png",   TextureConfig::EMetallicMap);
	//pFlat->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Roughness.png",  TextureConfig::ERoughnessMap);
	ENGINE.GetActorManagerPtr()->AddActor(pFlat, count++);

	Actor* pGrid = new Actor();
	pGrid->SetScale(160.0f, 160.0f, 160.0f);
	pGrid->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	pGrid->AddComponent<MeshComponent>();
	pGrid->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicLine, ShaderID::ELine, nullptr, FbxType::EDefault);
	ENGINE.GetActorManagerPtr()->AddActor(pGrid, count++);


	// ----------------------------------------------------------------------------------------------
	// 追加設定
	// ----------------------------------------------------------------------------------------------
	ENGINE.GetLightPtr()->Init(0, 0);
	ENGINE.GetLightPtr()->SetLightColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));
	ENGINE.GetLightPtr()->SetShininess(30.0f);

	ENGINE.GetCameraPtr()->SetTarget(pSphere);

	Settings::Renderer renderSettings = {
	false,   // shadow
	false,  // ssao
	false,   // deffered
	false   // cubemap
	};
	ENGINE.SetRendererSettings(renderSettings);

}

void SceneA::InitializeScene()
{
}

void SceneA::Update(float elapsed_time)
{
}

void SceneA::PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics)
{
}

void SceneA::Render(
	std::unique_ptr<GraphicsEngine>& p_graphics,
	float elapsed_time)
{
}

void SceneA::RenderUI()
{
}


SceneA::~SceneA()
{
}

