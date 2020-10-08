#include "SceneShadowMapTest.h"
#include "SceneManager.h"

#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Engine/CameraController.h"
#include "./Engine/DirectionalLight.h"
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/MeshRenderer.h"

#include "./Renderer/ShadowMap.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Skybox.h"


//@brief シャドウマップのテスト（Forwardレンダリング）

//----------------------------------------------------------------------------------------------------------------------------

SceneF::SceneF(SceneManager* manager, D3D::DevicePtr& device):Scene(manager, device)
{
    mNextScene = SceneID::SCENE_A;

    //--------------------------------------------------------------------------------
    // FORWARD
    //--------------------------------------------------------------------------------

    m_pToShadowShader = std::make_shared<Shader>();
    m_pToShadowShader->createShader(
        device,
        L"./Src/Shaders/ShadowMap.hlsl",
        L"./Src/Shaders/ShadowMap.hlsl",
        "VSshadow", "PSshadow", VEDType::VED_DEFAULT);

    m_pFromShadowShader = std::make_shared<Shader>();
    m_pFromShadowShader->createShader(
        device,
        L"./Src/Shaders/ShadowMap.hlsl",
        L"./Src/Shaders/ShadowMap.hlsl",
        "VSmain2", "PSmain", VEDType::VED_DEFAULT);

    m_pToShadowForSkinnedMesh = std::make_shared<Shader>();
    m_pToShadowForSkinnedMesh->createShader(
        device,
        L"./Src/Shaders/ShadowMap.hlsl",
        L"./Src/Shaders/ShadowMap.hlsl",
        "VSshadowSkinning", "PSshadow", VEDType::VED_SKINNED_MESH
    );

    m_pFromShadowForSkinnedMesh = std::make_shared<Shader>();
    m_pFromShadowForSkinnedMesh->createShader(
        device,
        L"./Src/Shaders/ShadowMap.hlsl",
        L"./Src/Shaders/ShadowMap.hlsl",
        "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);


    //----------------------------------------------------------------------------------------
    // テクスチャ初期化
    //----------------------------------------------------------------------------------------
    m_pShadowMap = std::make_unique<ShadowMap>(device);

    //----------------------------------------------------------------------------------------
    // プレファブ初期化
    //----------------------------------------------------------------------------------------

    m_pLightController->Init(0, 0);



    //----------------------------------------------------------------------------------------
    // アクター・コンポーネント初期化
    //----------------------------------------------------------------------------------------
    m_pPlayer = Actor::Initialize(ActorID::kPlayer);
    m_pPlayer->SetRotationQuaternion(0, 0, 0);
    //m_pPlayer->AddComponent(
    //    MeshComponent::MeshID::kSkinnedMesh,
    //    m_pRenderer,
    //    m_pFromShadowForSkinnedMesh,
    //    "./Data/Models/Female/Idle.fbx"
    //);
    m_pPlayer->AddComponent(
        MeshComponent::MeshID::kSkinnedMesh,
        m_pRenderer,
        m_pFromShadowForSkinnedMesh,
        "./Data/Models/ufooo/yh_g_walk.fbx"
    );
    m_pPlayer->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/ufooo/yh_g.fbm/g_n.png");
    m_pPlayer->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/ufooo/yh_g.fbm/g_h.png");

    m_pPlayer->SetAdditiveRotation(-90.0f, 0.0f, 0.0f);
    m_pPlayer->SetScale(0.05f, 0.05f, 0.05f);
    //m_pPlayer->SetPosition(Vector3(0, 2, 0));
    m_pPlayer->SetPosition(Vector3(0, 4, 0));

    m_pPlayer->GetComponent<MeshComponent>()->AddMotion("Walking", "./Data/Models/ufooo/yh_g_walk.fbx");

    //m_pPlayer->GetComponent<MeshComponent>()->AddMotion("Walking", "./Data/Models/Female/Walking.fbx");
    //m_pPlayer->GetComponent<MeshComponent>()->AddMotion("Idle", "./Data/Models/Female/Idle.fbx");
    //m_pPlayer->GetComponent<MeshComponent>()->AddMotion("Running", "./Data/Models/Female/Running.fbx");
    m_pActorManager->AddActor(m_pPlayer);

    m_pField = Actor::Initialize(ActorID::kNonPlayer);
    m_pField->SetPosition(Vector3(0, 0, 0));
    m_pField->SetScale(200.0f, 0.5f, 200.0f);
    m_pField->AddComponent(
        MeshComponent::MeshID::kBasicCube,
        m_pRenderer,
        m_pFromShadowShader,
        L"\0"
    );
    m_pActorManager->AddActor(m_pField);


    // 追跡カメラのターゲット設定
    m_pCameraController->SetTarget(m_pPlayer);

    m_pRenderer->AddShaderForShadow(m_pToShadowShader);
    m_pPlayer->GetComponent<MeshComponent>()->SetShader(m_pToShadowForSkinnedMesh, true);
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneF::Update(float elapsed_time)
{
    m_pCameraController->Update(elapsed_time);

    //m_pLight->Update(elapsed_time);
    m_pLightController->Update(elapsed_time);
    
    //if (InputPtr->OnKeyTrigger("2"))
    //{
    //    m_pPlayer->GetComponent<MeshComponent>()->Play("Idle");
    //}
    if (InputPtr->OnKeyTrigger("2"))
    {
        m_pPlayer->GetComponent<MeshComponent>()->Play("default");
    }

    if (InputPtr->OnKeyTrigger("3"))
    {
        m_pPlayer->GetComponent<MeshComponent>()->Play("Walking");
    }
    //if (InputPtr->OnKeyTrigger("4"))
    //{
    //    m_pPlayer->GetComponent<MeshComponent>()->Play("Running");
    //}
    

    m_pActorManager->Update(elapsed_time);
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneF::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> immContext = p_graphics->GetImmContextPtr();

    m_pBlender->SetBlendState(immContext, Blender::BLEND_ALPHA);

    //---------------------------------------------------------------------------------------------
    // シャドウマップ用意
    //---------------------------------------------------------------------------------------------
    m_pShadowMap->Activate(p_graphics, m_pLightController, m_pCameraController);

    m_pRenderer->RenderShadowMapQueue(immContext, elapsed_time, m_pCameraController);

    m_pShadowMap->Deactivate(p_graphics);

    //---------------------------------------------------------------------------------------------
    // 描画準備（カメラ・ライト）
    //---------------------------------------------------------------------------------------------
    m_pCameraController->SetMatrix(immContext);

    m_pLightController->SetDataForGPU(immContext, m_pCameraController);

    //---------------------------------------------------------------------------------------------
    // 描画
    //---------------------------------------------------------------------------------------------

    m_pRenderer->RenderQueue(immContext, elapsed_time);
}

//----------------------------------------------------------------------------------------------------------------------------
