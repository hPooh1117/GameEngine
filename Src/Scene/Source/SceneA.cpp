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
//#include "./Engine/MeshRenderer.h"
#include "./Engine/Settings.h"

#include "./Component/MoveRotationComponent.h"

#include "./Renderer/Skybox.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/sprite.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"



using namespace DirectX;


SceneA::SceneA(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device) :Scene(manager, device)
{
	mNextScene = SceneID::SCENE_B;


	// ----------------------------------------------------------------------------------------------
	// アクター・コンポーネント作成
	// ----------------------------------------------------------------------------------------------
	mpSphere = Actor::Initialize(ActorID::kPlayer);
	mpSphere->SetScale(5.0f, 5.0f, 5.0f);
	mpSphere->SetPosition(Vector3(0, 1, 0));
	mpSphere->AddComponent<MoveRotationComponent>();
	mpSphere->AddComponent<NewMeshComponent>();
	mpSphere->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EEnvironmentMap, nullptr, FbxType::EDefault);
	mpSphere->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/check.jpg", TextureConfig::EColorMap);
	mpSphere->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/Environment/Walk_Of_Fame/Mans_Outside_2k.hdr", TextureConfig::EEnvironmentMap);
	ENGINE.GetActorManagerPtr()->AddActor(mpSphere);

	mpEarth = Actor::Initialize(ActorID::kNonPlayer);
	mpEarth->SetScale(5.0f, 5.0f, 5.0f);
	mpEarth->SetPosition(Vector3(5, 1, 0));
	mpEarth->AddComponent<MoveRotationComponent>();
	mpEarth->AddComponent<NewMeshComponent>();
	mpEarth->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::ENormalMap, nullptr, FbxType::EDefault);
	mpEarth->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/earthmap.jpg", TextureConfig::EColorMap);
	mpEarth->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/earthnormal.jpg", TextureConfig::ENormalMap);
	mpEarth->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/earthbump.jpg", TextureConfig::EHeightMap);
	ENGINE.GetActorManagerPtr()->AddActor(mpEarth);

	mpToon = Actor::Initialize(ActorID::kEnemy);
	mpToon->SetScale(5, 5, 5);
	mpToon->SetPosition(Vector3(-5, 1, 0));
	mpToon->AddComponent<MoveRotationComponent>();
	mpToon->AddComponent<NewMeshComponent>();
	mpToon->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EToonGeo, nullptr, FbxType::EDefault);
	mpToon->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/Test.png", TextureConfig::EColorMap);
	ENGINE.GetActorManagerPtr()->AddActor(mpToon);

	mpFlat = Actor::Initialize(ActorID::kFloor);
	mpFlat->SetScale(0.5f, 0.5f, 0.5f);
	mpFlat->SetPosition(Vector3(0, 5, 0));
	mpFlat->AddComponent<MoveRotationComponent>();
	mpFlat->AddComponent<NewMeshComponent>();
	mpFlat->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_StaticMesh, ShaderID::EFlat, L"./Data/Models/OBJ/Mr.Incredible/Mr.Incredible.obj", FbxType::EMaya);
	//mpFlat->GetComponent<NewMeshComponent>()->SetMaterialColor(Vector4(0.8f, 0.1f, 0.1f, 1.0f));
	//mpFlat->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Base_Color.png", TextureConfig::EColorMap);
	//mpFlat->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg",			 TextureConfig::EEnvironmentMap);
	//mpFlat->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_3K_Metallic.png",   TextureConfig::EMetallicMap);
	//mpFlat->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Roughness.png",  TextureConfig::ERoughnessMap);
	ENGINE.GetActorManagerPtr()->AddActor(mpFlat);

	mpGrid = Actor::Initialize(ActorID::kNonPlayer0);
	mpGrid->SetScale(160.0f, 160.0f, 160.0f);
	mpGrid->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	mpGrid->AddComponent<NewMeshComponent>();
	mpGrid->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicLine, ShaderID::ELine, nullptr, FbxType::EDefault);
	ENGINE.GetActorManagerPtr()->AddActor(mpGrid);


	// ----------------------------------------------------------------------------------------------
	// 追加設定
	// ----------------------------------------------------------------------------------------------
	ENGINE.GetLightPtr()->Init(0, 0);
	ENGINE.GetLightPtr()->SetLightColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

	ENGINE.GetCameraPtr()->SetTarget(mpSphere);

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

