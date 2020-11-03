#include "SceneB.h"
//#include "SceneManager.h"

#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Component/MeshComponent.h"

#include "./Engine/GameSystem.h"
#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/PointLight.h"
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
//#include "./Engine/MeshRenderer.h"
#include "./Engine/UIRenderer.h"

#include "./Renderer/Shader.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Blender.h"
#include "./Renderer/ShadowMap.h"
#include "./Renderer/MultiRenderTarget.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Skybox.h"


using namespace DirectX;

SceneB::SceneB(SceneManager * manager, Microsoft::WRL::ComPtr<ID3D11Device>& device):Scene(manager, device)
{
    mNextScene = SceneID::SCENE_C;


    ENGINE.GetLightPtr()->Init(4, 0);
    ENGINE.GetCameraPtr()->SetTarget(m_pPlayer);

    ENGINE.CastShadow();
    ENGINE.SetIsDefferedRendering(true);
    ENGINE.SetSSAO(false);



// ----------------------------------------------------------------------------------------------
// アクター・コンポーネント作成
// ----------------------------------------------------------------------------------------------
    m_pPlayer = Actor::Initialize(ActorID::kPlayer);
    m_pPlayer->SetScale(5, 5, 5);
    m_pPlayer->SetPosition(Vector3(0, 4, 0));
    m_pPlayer->AddComponent<NewMeshComponent>();
    m_pPlayer->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicSphere, ShaderType::EDefferedNormal, nullptr, FbxType::EDefault);
    m_pPlayer->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/earthmap.jpg", 0);
    m_pPlayer->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/earthnormal.jpg", 1);
    m_pPlayer->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Images/earthbump.jpg", 2);
    m_pPlayer->GetComponent<NewMeshComponent>()->RegistAdditionalShader(ShaderType::EToShadow, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(m_pPlayer);

    m_pCat = Actor::Initialize(ActorID::kNonPlayer);
    m_pCat->SetPosition(Vector3(4, 1, 0));
    m_pCat->SetScale(0.5f, 0.5f, 0.5f);
    m_pCat->AddComponent<NewMeshComponent>();
    m_pCat->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_SkinnedMesh, ShaderType::EDefferedShadowForSkinned, L"./Data/Models/oswell/cat_mdl.fbx", FbxType::EMaya);
    m_pCat->GetComponent<NewMeshComponent>()->RegistAdditionalShader(ShaderType::EToShadow, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(m_pCat);

    m_pOswell = Actor::Initialize(ActorID::kEnemy);
    m_pOswell->SetPosition(Vector3(-4, 3, 0));
    m_pOswell->SetScale(0.05f, 0.05f, 0.05f);
    m_pOswell->AddComponent<NewMeshComponent>();
    m_pOswell->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_SkinnedMesh, ShaderType::EDefferedShadowForSkinning, L"./Data/Models/oswell/oswell_test1.fbx", FbxType::EMaya);
    m_pOswell->GetComponent<NewMeshComponent>()->RegistMotion("Move", L"./Data/Models/oswell/oswell_movemove.fbx");
    m_pOswell->GetComponent<NewMeshComponent>()->RegistAdditionalShader(ShaderType::EToShadowForSkinning, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(m_pOswell);

    m_pShiba = Actor::Initialize(ActorID::kNonPlayer0);
    m_pShiba->SetPosition(Vector3(0, 0.5f, -4));
    m_pShiba->AddComponent < NewMeshComponent>();
    m_pShiba->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_SkinnedMesh, ShaderType::EDefferedNormalForSkinned, L"./Data/Models/0810/shiba_mdl.fbx", FbxType::EMaya);
    m_pShiba->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Models/0810/shina_N.png", 1);
    m_pShiba->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Models/0810/shiba_H.png", 2);
    m_pShiba->GetComponent<NewMeshComponent>()->RegistAdditionalShader(ShaderType::EToShadow, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(m_pShiba);

    m_pField = Actor::Initialize(ActorID::kFloor);
    m_pField->SetPosition(Vector3(0, 0, 0));
    m_pField->SetScale(200, 200, 200);
    m_pField->AddComponent<NewMeshComponent>();
    m_pField->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_StaticMesh, ShaderType::EDefferedNormal, L"./Data/Models/OBJ/sea/sea.obj", FbxType::EDefault);
    m_pField->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Models/OBJ/sea/Nsea.png", 1);
    m_pField->GetComponent<NewMeshComponent>()->RegistTexture(L"./Data/Models/OBJ/sea/Hsea.png", 2);
    m_pField->GetComponent<NewMeshComponent>()->RegistAdditionalShader(ShaderType::EToShadow, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(m_pField);
}

void SceneB::InitializeScene()
{
}

void SceneB::Update(float elapsed_time)
{
    if (InputPtr.OnKeyDown("X"))
    {
        m_pOswell->GetComponent<NewMeshComponent>()->Play("Move");
    }

    static float angle = XM_PI / 4;
    angle += elapsed_time;
    float s = sinf(angle) * 5.0f;
    float c = cosf(angle) * 5.0f;
    ENGINE.GetLightPtr()->SetPointData(0, Vector3(-s, 3, c), Vector4(0.7f, 0.5f, 0.4f, 1.0f), 15.0f);
    ENGINE.GetLightPtr()->SetPointData(1, Vector3(-c, 3, s), Vector4(0.7f, 0.3f, 0.4f, 1.0f), 15.0f);
    ENGINE.GetLightPtr()->SetPointData(2, Vector3(s, 3, -c), Vector4(0.3f, 0.4f, 0.7f, 1.0f), 15.0f);
    ENGINE.GetLightPtr()->SetPointData(3, Vector3(c, 3, s), Vector4(0.7f, 0.3f, 0.3f, 1.0f), 15.0f);

}

void SceneB::Render(std::unique_ptr<GraphicsEngine>& p_graphics,
    float elapsed_time)
{
}

SceneB::~SceneB()
{
}
