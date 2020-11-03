#include "SceneA.h"
//#include "SceneManager.h"

#include "./Application/Application.h"

#include "./Component/MeshComponent.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/DirectionalLight.h"
#include "./Engine/GameSystem.h"
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/UIRenderer.h"
//#include "./Engine/MeshRenderer.h"

#include "./Component/MoveRotationComponent.h"

#include "./Renderer/Skybox.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/sprite.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"



using namespace DirectX;

#pragma region OLDIMPL

SceneA::SceneA(SceneManager * manager, Microsoft::WRL::ComPtr<ID3D11Device>& device):Scene(manager, device)
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
    mpSphere->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::EEnvironmentMap, nullptr, FbxType::EDefault);
    mpSphere->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/check.jpg", 0);
    mpSphere->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg", 1);
    ENGINE.GetActorManagerPtr()->AddActor(mpSphere);

    mpEarth = Actor::Initialize(ActorID::kNonPlayer);
    mpEarth->SetScale(5.0f, 5.0f, 5.0f);
    mpEarth->SetPosition(Vector3(5, 1, 0));
    mpEarth->AddComponent<MoveRotationComponent>();
    mpEarth->AddComponent<NewMeshComponent>();
    mpEarth->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::ENormalMap, nullptr, FbxType::EDefault);
    mpEarth->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/earthmap.jpg", 0);
    mpEarth->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/earthnormal.jpg", 1);
    mpEarth->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/earthbump.jpg", 2);
    ENGINE.GetActorManagerPtr()->AddActor(mpEarth);

    mpToon = Actor::Initialize(ActorID::kEnemy);
    mpToon->SetScale(5, 5, 5);
    mpToon->SetPosition(Vector3(-5, 1, 0));
    mpToon->AddComponent<MoveRotationComponent>();
    mpToon->AddComponent<NewMeshComponent>();
    mpToon->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::EToonGeo, nullptr, FbxType::EDefault);
    mpToon->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/Test.png", 0);
    ENGINE.GetActorManagerPtr()->AddActor(mpToon);

    //m_pPBRTest = Actor::Initialize(ActorID::kFloor);
    //m_pPBRTest->SetScale(5, 5, 5);
    //m_pPBRTest->SetPosition(Vector3(0, 10, 0));
    //m_pPBRTest->AddComponent<MoveRotationComponent>();
    //m_pPBRTest->AddComponent<NewMeshComponent>();
    //m_pPBRTest->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::EForwardPBR, nullptr, FbxType::EDefault);
    //m_pPBRTest->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Base_Color.png", 0);
    //m_pPBRTest->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg", 1);
    //m_pPBRTest->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_3K_Metallic.png", 2);
    //m_pPBRTest->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Roughness.png", 3);
    //ENGINE.GetActorManagerPtr()->AddActor(m_pPBRTest);
    

    mpGrid = Actor::Initialize(ActorID::kNonPlayer0);
    mpGrid->SetScale(160.0f, 160.0f, 160.0f);
    mpGrid->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
    mpGrid->AddComponent<NewMeshComponent>();
    mpGrid->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicLine, ShaderType::ELine, nullptr, FbxType::EDefault);
    ENGINE.GetActorManagerPtr()->AddActor(mpGrid);


// ----------------------------------------------------------------------------------------------
// 追加設定
// ----------------------------------------------------------------------------------------------
   ENGINE.GetLightPtr()->Init(0, 0);
   ENGINE.GetCameraPtr()->SetTarget(mpSphere);

   ENGINE.SetIsDefferedRendering(false);
   ENGINE.SetSSAO(false);
   ENGINE.NotCastShadow();
}

void SceneA::InitializeScene()
{
}

void SceneA::Update(float elapsed_time)
{
}

void SceneA::Render(
    std::unique_ptr<GraphicsEngine>& p_graphics,
    float elapsed_time)
{

    ENGINE.GetUIRenderer()->SetNextWindowSettings(Vector2(0, SCREEN_HEIGHT - 200), Vector2(300, 200));
    ENGINE.GetUIRenderer()->BeginRenderingNewWindow("PBR");
    m_pPBRTest->GetComponent<NewMeshComponent>()->RenderUI();
    ENGINE.GetUIRenderer()->FinishRenderingWindow();

}


SceneA::~SceneA()
{
}

#pragma endregion
