#include "SceneE.h"
#include "SceneManager.h"

#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Component/MeshComponent.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/GameSystem.h"
#include "./Engine/Settings.h"

#include "./Component/MoveRotationComponent.h"

#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Blender.h"
#include "./Renderer/ShadowMap.h"
#include "./Renderer/AmbientOcclusion.h"
#include "./Renderer/MultiRenderTarget.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Skybox.h"


//----------------------------------------------------------------------------------------------------------------------------

SceneE::SceneE(SceneManager* manager, D3D::DevicePtr& device) :Scene(manager, device)
{
	mNextScene = SceneID::SCENE_F;

    ENGINE.GetLightPtr()->Init(0, 0);
    ENGINE.GetLightPtr()->SetLightColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    InitializeActors();


    ENGINE.GetCameraPtr()->SetTarget(mpShiba);

    Settings::Renderer renderSettings = {
    true,   // shadow
    true,  // ssao
    true,   // deffered
    false   // cubemap
    };
    ENGINE.SetRendererSettings(renderSettings);


}

void SceneE::InitializeScene()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneE::InitializeActors()
{
//----------------------------------------------------------------------------------------
// アクター・コンポーネント初期化
//----------------------------------------------------------------------------------------
    mpCat = Actor::Initialize(ActorID::kNonPlayer);
    mpCat->SetPosition(Vector3(1.0f, 1, -1.5f - 5.0f));
    mpCat->SetScale(2, 4, 2);
    mpCat->AddComponent<NewMeshComponent>();
    mpCat->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCylinder, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    mpCat->GetComponent<NewMeshComponent>()->SetMaterialColor(Vector4(0.7f, 0.3f, 0.5f, 1.0f));
    mpCat->GetComponent<NewMeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(mpCat);

    mpOswell = Actor::Initialize(ActorID::kEnemy);
    mpOswell->SetPosition(Vector3(-2.5f, 3.0f, 0.0f - 5.0f));
    mpOswell->SetScale(0.05f, 0.05f, 0.05f);
    mpOswell->AddComponent<NewMeshComponent>();
    mpOswell->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::EDefferedNormalForSkinning, L"./Data/Models/oswell/oswell_test1.fbx", FbxType::EMaya);
    mpOswell->GetComponent<NewMeshComponent>()->RegisterMotion("Move", L"./Data/Models/oswell/oswell_movemove.fbx");
    mpOswell->GetComponent<NewMeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadowForSkinning, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(mpOswell);

    mpShiba = Actor::Initialize(ActorID::kPlayer);
    mpShiba->SetScale(3, 3, 3);
    mpShiba->SetPosition(Vector3(0, 1.5f, -4 - 4.0f));
    mpShiba->AddComponent<NewMeshComponent>();
    mpShiba->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    mpShiba->GetComponent<NewMeshComponent>()->SetMaterialColor(Vector4(0.4f, 0.3f, 0.5f, 1.0f));
    mpShiba->GetComponent<NewMeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);

    ENGINE.GetActorManagerPtr()->AddActor(mpShiba);

    mpField = Actor::Initialize(ActorID::kFloor);
    mpField->SetPosition(Vector3(0, 0, 0));
    mpField->SetScale(50, 0.5f, 50);
    mpField->AddComponent<NewMeshComponent>();
    mpField->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    //mpField->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/Metal_Panel_Color.jpg", TextureConfig::EColorMap);
    mpField->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/Metal_Panel_Normal.jpg", TextureConfig::ENormalMap);
    mpField->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/Metal_Panel_Height.png", TextureConfig::EHeightMap);
    mpField->GetComponent<NewMeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(mpField);


    //std::shared_ptr<Actor> m_pWall = Actor::Create(ActorID::kNonPlayer0);
    //m_pWall->SetPosition(Vector3(0, 25, 25));
    //m_pWall->SetScale(50, 50, 0.5f);
    //m_pWall->AddComponent<NewMeshComponent>();
    //m_pWall->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    //m_pWall->GetComponent<NewMeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);
    //ENGINE.GetActorManagerPtr()->AddActor(m_pWall);

    //std::shared_ptr<Actor> m_pWall1 = Actor::Create(ActorID::kNonPlayer0 + 1);
    //m_pWall1->SetPosition(Vector3(-25, 25, 0));
    //m_pWall1->SetScale(0.5f, 50.0f, 50.0f);
    //m_pWall1->AddComponent<NewMeshComponent>();
    //m_pWall1->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    //m_pWall1->GetComponent<NewMeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);
    //ENGINE.GetActorManagerPtr()->AddActor(m_pWall1);

    //std::shared_ptr<Actor> m_pWall2 = Actor::Create(ActorID::kNonPlayer0 + 2);
    //m_pWall2->SetPosition(Vector3(25, 25, 0));
    //m_pWall2->SetScale(0.5f, 50.0f, 50.0f);
    //m_pWall2->AddComponent<NewMeshComponent>();
    //m_pWall2->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    //m_pWall2->GetComponent<NewMeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);
    //ENGINE.GetActorManagerPtr()->AddActor(m_pWall2);

}

//----------------------------------------------------------------------------------------------------------------------------

void SceneE::Update(float elapsed_time)
{
    //if (InputPtr.OnKeyDown("X"))
    //{
    //    mpOswell->GetComponent<NewMeshComponent>()->Play("Move");
    //}

    ENGINE.GetActorManagerPtr()->Update(elapsed_time);
}

void SceneE::PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics)
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneE::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
}

void SceneE::RenderUI()
{
}

//----------------------------------------------------------------------------------------------------------------------------

SceneE::~SceneE()
{
}


//----------------------------------------------------------------------------------------------------------------------------
