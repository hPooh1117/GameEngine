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
    ENGINE.GetLightPtr()->SetShininess(10.0f);
    InitializeActors();



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
    int count = 0;

    Actor* pCylinder = new Actor();
    pCylinder->SetPosition(Vector3(5.0f, 1, -3.5f));
    pCylinder->SetScale(2, 4, 2);
    pCylinder->AddComponent<MeshComponent>();
    pCylinder->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCylinder, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    pCylinder->GetComponent<MeshComponent>()->SetMaterialColor(Vector4(0.7f, 0.3f, 0.5f, 1.0f));
    pCylinder->GetComponent<MeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(pCylinder, count++);

    Actor* pOswell = new Actor();
    pOswell->SetPosition(Vector3(0.0f, 2.75f, 0.0f));
    pOswell->SetScale(0.05f, 0.05f, 0.05f);
    pOswell->AddComponent<MeshComponent>();
    pOswell->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::EDefferedNormalForSkinning, L"./Data/Models/oswell/oswell_test1.fbx", FbxType::EMaya);
    pOswell->GetComponent<MeshComponent>()->RegisterMotion("Move", L"./Data/Models/oswell/oswell_movemove.fbx");
    pOswell->GetComponent<MeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadowForSkinning, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(pOswell, count++);

    Actor* pSphere = new Actor();
    pSphere->SetScale(3, 3, 3);
    pSphere->SetPosition(Vector3(3, 1.5f, -5.0f));
    pSphere->AddComponent<MeshComponent>();
    pSphere->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    pSphere->GetComponent<MeshComponent>()->SetMaterialColor(Vector4(0.4f, 0.3f, 0.5f, 1.0f));
    pSphere->GetComponent<MeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);

    ENGINE.GetActorManagerPtr()->AddActor(pSphere, count++);

    Actor* pCylinder1 = new Actor();
    pCylinder1->SetPosition(Vector3(-5.0f, 1, -3.5f));
    pCylinder1->SetScale(2, 4, 2);
    pCylinder1->AddComponent<MeshComponent>();
    pCylinder1->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCylinder, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    pCylinder1->GetComponent<MeshComponent>()->SetMaterialColor(Vector4(0.7f, 0.3f, 0.5f, 1.0f));
    pCylinder1->GetComponent<MeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(pCylinder1, count++);

    Actor* pSphere1 = new Actor();
    pSphere1->SetScale(3, 3, 3);
    pSphere1->SetPosition(Vector3(-3, 1.5f, -5.0f));
    pSphere1->AddComponent<MeshComponent>();
    pSphere1->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    pSphere1->GetComponent<MeshComponent>()->SetMaterialColor(Vector4(0.4f, 0.3f, 0.5f, 1.0f));
    pSphere1->GetComponent<MeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);

    ENGINE.GetActorManagerPtr()->AddActor(pSphere1, count++);

    Actor* pField = new Actor();
    pField->SetPosition(Vector3(0, 0, 0));
    pField->SetScale(50, 0.5f, 50);
    pField->AddComponent<MeshComponent>();
    pField->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::EDefferedNormal, nullptr, FbxType::EDefault);
    //pField->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/Metal_Panel_Color.jpg", TextureConfig::EColorMap);
    pField->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/Metal_Panel_Normal.jpg", TextureConfig::ENormalMap);
    pField->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/Metal_Panel_Height.png", TextureConfig::EHeightMap);
    pField->GetComponent<MeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(pField, count++);


    ENGINE.GetCameraPtr()->SetTarget(pSphere);

}

//----------------------------------------------------------------------------------------------------------------------------

void SceneE::Update(float elapsed_time)
{
    //if (InputPtr.OnKeyDown("X"))
    //{
    //    pOswell->GetComponent<MeshComponent>()->Play("Move");
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
