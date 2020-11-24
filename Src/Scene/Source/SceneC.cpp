#include "SceneC.h"
#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Component/MeshComponent.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/PointLight.h"
#include "./Engine/SpotLight.h"
#include "./Engine/DirectionalLight.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/LightController.h"
#include "./Engine/GameSystem.h"
#include "./Engine/Settings.h"

#include "./Component/MoveRotationComponent.h"

#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Blender.h"
#include "./Renderer/ShadowMap.h"
#include "./Renderer/MultiRenderTarget.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Shader.h"


using namespace DirectX;

SceneC::SceneC(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device) :Scene(manager, device)
{
    mNextScene = SceneID::SCENE_D;


// ----------------------------------------------------------------------------------------------
// アクター・コンポーネント作成
// ----------------------------------------------------------------------------------------------
    mpPlayer = Actor::Initialize(ActorID::kPlayer);
    mpPlayer->AddComponent<MoveRotationComponent>();
    mpPlayer->AddComponent<NewMeshComponent>();
    mpPlayer->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::ESpotLightBump, nullptr, FbxType::EDefault);
    mpPlayer->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/earthmap.jpg", TextureConfig::EColorMap);
    mpPlayer->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/earthnormal.jpg", TextureConfig::ENormalMap);
    mpPlayer->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/earthbump.jpg", TextureConfig::EHeightMap);
    mpPlayer->SetScale(5, 5, 5);
    mpPlayer->SetPosition(Vector3(0, 4, 0));
    ENGINE.GetActorManagerPtr()->AddActor(mpPlayer);

    mpCat = Actor::Initialize(ActorID::kNonPlayer);
    mpCat->AddComponent<NewMeshComponent>();
    mpCat->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::ESpotLightPhong, L"./Data/Models/oswell/cat_mdl.fbx", FbxType::EMaya);
    mpCat->SetPosition(Vector3(4, 1, 0));
    mpCat->SetScale(0.5f, 0.5f, 0.5f);
    ENGINE.GetActorManagerPtr()->AddActor(mpCat);

    mpOswell = Actor::Initialize(ActorID::kEnemy);
    mpOswell->AddComponent<NewMeshComponent>();
    mpOswell->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::ESpotLightPhongForSkinning, L"./Data/Models/oswell/oswell_test1.fbx", FbxType::EMaya);
    mpOswell->GetComponent<NewMeshComponent>()->RegisterMotion("Move", L"./Data/Models/oswell/oswell_movemove.fbx");
    mpOswell->SetPosition(Vector3(-4, 3, 0));
    mpOswell->SetScale(0.05f, 0.05f, 0.05f);
    ENGINE.GetActorManagerPtr()->AddActor(mpOswell);

    mpShiba = Actor::Initialize(ActorID::kNonPlayer0);
    mpShiba->AddComponent<NewMeshComponent>();
    mpShiba->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::ESpotLightBump, L"./Data/Models/0810/shiba_mdl.fbx", FbxType::EMaya);
    mpShiba->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Models/0810/shina_N.png", TextureConfig::ENormalMap);
    mpShiba->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Models/0810/shiba_H.png", TextureConfig::EHeightMap);
    mpShiba->SetPosition(Vector3(0, 0.5f, -4));
    ENGINE.GetActorManagerPtr()->AddActor(mpShiba);

    mpField = Actor::Initialize(ActorID::kFloor);
    mpField->AddComponent<NewMeshComponent>();
    mpField->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_StaticMesh, ShaderID::ESpotLightOcean, L"./Data/Models/OBJ/sea/sea.obj", FbxType::EDefault);
    mpField->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Models/OBJ/sea/Nsea.png", TextureConfig::ENormalMap);
    mpField->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Models/OBJ/sea/Hsea.png", TextureConfig::EHeightMap);
    mpField->SetPosition(Vector3(0, 0, 0));
    mpField->SetScale(200, 200, 200);
    ENGINE.GetActorManagerPtr()->AddActor(mpField);


   ENGINE.GetLightPtr()->Init(0, 4);
   ENGINE.GetLightPtr()->SetLightColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
   ENGINE.GetLightPtr()->SetSpotData(0, Vector3(-5, 5, -5), Vector4(0.9f, 0.2f, 0.2f, 1.0f), Vector3(1.0f, -0.2f, 0.0f), 50.0f, 0.99f, 0.8f);
   ENGINE.GetLightPtr()->SetSpotData(1, Vector3(-5, 5, 5), Vector4(0.2f, 0.2f, 0.9f, 1.0f), Vector3(-1.0f, -0.2f, 0.0f), 50.0f, 0.99f, 0.8f);
   ENGINE.GetLightPtr()->SetSpotData(2, Vector3(5, 5, -5), Vector4(0.2f, 0.9f, 0.2f, 1.0f), Vector3(0.0f, -0.2f, 1.0f), 50.0f, 0.99f, 0.8f);
   ENGINE.GetLightPtr()->SetSpotData(3, Vector3(5, 5, 5), Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector3(0.0f, -0.2f, -1.0f), 50.0f, 0.99f, 0.8f);


    ENGINE.GetCameraPtr()->SetTarget(mpSphere);

    Settings::Renderer renderSettings = {
    false,   // shadow
    false,  // ssao
    false,   // deffered
    false   // cubemap
    };
    ENGINE.SetRendererSettings(renderSettings);


}

void SceneC::InitializeScene()
{
}

void SceneC::Update(float elapsed_time)
{
    //if (InputPtr.OnKeyDown("X"))
    //{
    //    mpOswell->GetComponent<NewMeshComponent>()->Play("Move");
    //}
}

void SceneC::PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics)
{
}

void SceneC::Render(std::unique_ptr<GraphicsEngine>& p_graphics,
    float elapsed_time)
{
}

SceneC::~SceneC()
{
}
