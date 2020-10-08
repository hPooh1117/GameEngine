#include "Scene01.h"
#include "SceneManager.h"

#include "./Application/Application.h"

#include "./Engine/CameraController.h"
#include "./Engine/TraceCamera.h"
#include "./Engine/CustomLight.h"
#include "./Engine/LightController.h"
#include "./Engine/PhysicsManager.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/MeshRenderer.h"

#include "./Component/MoveRotationComponent.h"
#include "./Component/MoveRoundTrip.h"
#include "./Component/CCPlanetResident.h"
#include "./Component/CCPlayerMoveForPlanet.h"
#include "./Component/CCPlayerLightController.h"
#include "./Component/StaticComponent.h"
#include "./Component/SphereColliderComponent.h"
#include "./Component/RayColliderComponent.h"
#include "./Component/CapsuleColliderComponent.h"
#include "./Component/MeshComponent.h"
#include "./Component/CCPhysicalMove.h"
#include "./Component/CCPlayerMove.h"
#include "./Component/CCEnemyMove.h"
#include "./Component/CCSphereCollider.h"
#include "./Component/CCCapsuleCollider.h"
#include "./Component/CCTargetPulledUp.h"
#include "./Component/CCPlayerCollector.h"

#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Shader.h"
#include "./Renderer/sprite.h"


//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

Scene01::Scene01(SceneManager * manager, Microsoft::WRL::ComPtr<ID3D11Device>& device):Scene(manager, device)
{
    // 次のシーン設定
    mNextScene = SceneID::SCENETITLE;

// ----------------------------------------------------------------------------------------------
// プレファブ作成
// ----------------------------------------------------------------------------------------------
    // スプライトフォント初期設定 
    mFont = std::make_unique<Sprite>(device, L".Data/Fonts/font0.png");
   
    // ライト初期設定
    m_pLightController->Init(4, 4);

    // 物理管理クラス初期設定
    m_pPhysicsManager = std::make_unique<PhysicsManager>();


// ----------------------------------------------------------------------------------------------
// シェーダー作成
// ----------------------------------------------------------------------------------------------
    m_pLambert = std::make_shared<Shader>();
    m_pLambert->createShader(
        device,
        L"../../Shaders/BasicShader.hlsl",
        L"../../Shaders/BasicShader.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT
    );

    m_pPhongShader = std::make_shared<Shader>();
    m_pPhongShader->createShader(
        device,
        L"../../Shaders/CustomLight.hlsl",
        L"../../Shaders/CustomLight.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT);

    m_pPhongSkinned = std::make_shared<Shader>();
    m_pPhongSkinned->createShader(
        device,
        L"../../Shaders/CustomLight.hlsl",
        L"../../Shaders/CustomLight.hlsl",
        "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);

    m_pLineShader = std::make_shared<Shader>();
    m_pLineShader->createShader(
        device,
        L"../../Shaders/Line.hlsl",
        L"../../Shaders/Line.hlsl",
        "VSMainDuplicate", "PSmain", VEDType::VED_GEOMETRIC_PRIMITIVE
    );
    m_pLineShader->createGeometryShader(
        device,
        L"../../Shaders/Line.hlsl",
        "GSmainDuplicate"
    );

    m_pSpriteShader = std::make_shared<Shader>();
    m_pSpriteShader->createShader(
        device,
        L"../../Shaders/FromGBuffer.hlsl",
        L"../../Shaders/FromGBuffer.hlsl",
        "VSmain", "PSmain", VEDType::VED_SPRITE
    );


// ----------------------------------------------------------------------------------------------
// アクター・コンポーネント作成
// ----------------------------------------------------------------------------------------------
    m_pPlayer = Actor::Initialize(ActorID::kPlayer);
    m_pPlayer->SetPosition(Vector3(0, 5, 0));
    m_pPlayer->SetScale(0.1f, 0.1f, 0.1f);
    m_pPlayer->SetAdditiveRotation(-90.0f, 0, 0);
    m_pPlayer->SetRotationQuaternion(0, 0, 0);
    m_pPlayer->AddComponent<CCPlayerMoveForPlanet>();
    m_pPlayer->AddComponent<CCPlayerLightController>();
    m_pPlayer->AddComponent(
        MeshComponent::MeshID::kSkinnedMesh,
        m_pRenderer,
        m_pPhongSkinned,
        "./Data/Models/ufooo/yh_ufo.fbx"
    );
    //std::shared_ptr<MeshComponent> mesh = m_pPlayer->AddComponent(
    //    MeshComponent::MeshID::kBasicCapsule,
    //    m_pRenderer,
    //    m_pLambert
    //    );
    m_pPlayer->AddComponent<CCCapsuleCollider>();

    //mesh->SetColliderData(m_pPlayer->GetComponent<CCCapsuleCollider>());
    //mesh->SetWireframeMode();
    m_pPlayer->GetComponent<CCCapsuleCollider>()->SetOffset(Vector3(0, -0.4f, 0));
    m_pPlayer->GetComponent<CCCapsuleCollider>()->SetScale(Vector3(4.2f, 4.2f, 4.2f));
    m_pPlayer->AddComponent<CCPlayerCollector>();
    m_pActorManager->AddActor(m_pPlayer);
    
    m_pEnemy = Actor::Initialize(ActorID::kEnemy);
    m_pEnemy->SetPosition(Vector3(5, 1, 0));
    m_pEnemy->SetScale(0.1f, 0.1f, 0.1f);
    m_pEnemy->SetAdditiveRotation(-90.0f, 0, 0);
    m_pEnemy->SetRotationQuaternion(0, 0, 0);

    m_pEnemy->AddComponent<CCPlanetResident>();
    m_pEnemy->AddComponent<CCTargetPulledUp>()->SetPlayer(m_pPlayer);
    m_pEnemy->AddComponent<CCSphereCollider>()->ResetRadius(0.5f);
    m_pEnemy->AddComponent(
        MeshComponent::MeshID::kSkinnedMesh,
        m_pRenderer,
        m_pPhongSkinned,
        "./Data/Models/ufooo/yh_g.fbx"
    );

    //m_pEnemy->GetComponent<MeshComponent>()->SetColor(Vector4(0.0f, 0.1f, 0.4f, 1.0f));
    m_pActorManager->AddActor(m_pEnemy);

    for (auto i = 0; i < 8; ++i)
    {
        m_pTargets[i] = Actor::Initialize(ActorID::kNonPlayer0 + i);
        m_pTargets[i]->SetPosition(Vector3(25.0f * 0.5f * cosf(i * XM_PIDIV4 * 0.5f), 0/* + - i * 50.0f / 8.0f*/, 25.0f * 0.5f * sinf(i * XM_PIDIV4 * 0.5f)));
        m_pTargets[i]->SetScale(.05f, .05f, .05f);
        m_pTargets[i]->SetAdditiveRotation(-90.0f, 0, 0);
        m_pTargets[i]->SetRotationQuaternion(0, 0, 0);
        m_pTargets[i]->AddComponent<CCPlanetResident>();
        m_pTargets[i]->AddComponent<CCTargetPulledUp>()->SetPlayer(m_pPlayer);
        m_pTargets[i]->AddComponent<CCSphereCollider>()->ResetRadius(0.5f);
        m_pTargets[i]->AddComponent(
            MeshComponent::MeshID::kSkinnedMesh,
            m_pRenderer,
            m_pPhongSkinned,
            "./Data/Models/ufooo/yh_cat.fbx"
        );
        m_pActorManager->AddActor(m_pTargets[i]);

    }
    m_pTarget = Actor::Initialize(ActorID::kNonPlayer);
    m_pTarget->SetPosition(Vector3(-5, 1, 0));
    m_pTarget->SetScale(.05f, .05f, .05f);
    m_pTarget->SetAdditiveRotation(-90.0f, 0, 0);
    m_pTarget->SetRotationQuaternion(0, 0, 0);
    m_pTarget->AddComponent<CCPlanetResident>();
    m_pTarget->AddComponent<CCTargetPulledUp>()->SetPlayer(m_pPlayer);
    m_pTarget->AddComponent<CCSphereCollider>()->ResetRadius(0.5f);
    m_pTarget->AddComponent(
        MeshComponent::MeshID::kSkinnedMesh,
        m_pRenderer,
        m_pPhongShader,
        "./Data/Models/ufooo/yh_cat.fbx"
    );
   
    m_pTarget->GetComponent<MeshComponent>()->SetColor(Vector4(0.5f, 0.1f, 0.1f, 1.0f));
    m_pActorManager->AddActor(m_pTarget);

    m_pField = Actor::Initialize(ActorID::kFloor);
    m_pField->AddComponent(
        MeshComponent::MeshID::kBasicSphere,
        m_pRenderer,
        m_pPhongShader,
        L"./Data/Images/earthmap.jpg"
    );
    //m_pField->GetComponent<MeshComponent>()->SetWireframeMode();
    m_pField->SetScale(40.0f, 40.0f, 40.0f);
    m_pField->SetPosition(Vector3(0, -20.0f, 0));
    m_pField->AddComponent<StaticComponent>();
    m_pField->AddComponent<CCSphereCollider>()->ResetColliderType(ColliderType::kC_Planet);

    m_pActorManager->AddActor(m_pField);

#ifdef _DEBUG
    m_pGrid = Actor::Initialize(ActorID::kPlane);
    m_pGrid->AddComponent(
        MeshComponent::MeshID::kBasicLine,
        m_pRenderer,
        m_pLineShader
    );
    m_pGrid->SetScale(160.0f, 160.0f, 160.0f);
    m_pGrid->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

#endif

// ----------------------------------------------------------------------------------------------
// 追加設定
// ----------------------------------------------------------------------------------------------
    // 追跡カメラのターゲット設定
    m_pCameraController->SetTarget(m_pPlayer);

    // ライトの追加設定
    m_pPlayer->GetComponent<CCPlayerLightController>()->SetLight(m_pLightController);
    m_pPlayer->GetComponent<CCPlayerLightController>()->RegisterActorAsPlanet(m_pField);

    // 物理管理クラスの初期設定
    m_pPhysicsManager->Init(m_pActorManager);

    // 地形Actorを種々のActorに登録
    m_pPlayer->GetComponent<CCPlayerMoveForPlanet>()->RegistActorAsPlanet(m_pField);
    m_pEnemy->GetComponent<CCPlanetResident>()->RegistActorAsPlanet(m_pField);
    for (auto i = 0; i < 8; ++i)
    {
        m_pTargets[i]->GetComponent<CCPlanetResident>()->RegistActorAsPlanet(m_pField);
    }
    m_pTarget->GetComponent<CCPlanetResident>()->RegistActorAsPlanet(m_pField);

    // UI描画クラスのキューに追加
#ifdef _DEBUG
    std::shared_ptr<UIClient> player(m_pPlayer->GetComponent<CCPlayerMoveForPlanet>());
    m_pUIRenderer->SetInQueue("Player Move", player);
    std::shared_ptr<UIClient> lightController(m_pPlayer->GetComponent<CCPlayerLightController>());
    m_pUIRenderer->SetInQueue("Player Light Controll", lightController);
    //std::shared_ptr<UIClient> target(m_pTarget->GetComponent<CCPlanetResident>());
    //m_pUIRenderer->SetInQueue("Target", target);
    //std::shared_ptr<UIClient> pulledUp(m_pTarget->GetComponent<CCTargetPulledUp>());
    //m_pUIRenderer->SetInQueue("Pulled Up", pulledUp);
    m_pUIRenderer->SetInQueue("Light", m_pLightController);
    std::shared_ptr<UIClient> camera = m_pCameraController;
    m_pUIRenderer->SetInQueue("Camera", camera);
    std::shared_ptr<UIClient> trace = std::static_pointer_cast<TraceCamera>(m_pCameraController->GetCameraPtr(1));
    m_pUIRenderer->SetInQueue("Trace", trace);
#endif
}

//----------------------------------------------------------------------------------------------------------------------------

void Scene01::Update(float elapsed_time)
{
// 更新処理
    m_pCameraController->Update(elapsed_time);

    //m_pLight->Update(elapsed_time);
    m_pLightController->Update(elapsed_time);

    m_pActorManager->Update(elapsed_time);

    m_pPhysicsManager->DetectCollision(m_pActorManager/*, elapsed_time*/);


}

//----------------------------------------------------------------------------------------------------------------------------

void Scene01::Render(
    std::unique_ptr<GraphicsEngine>& p_graphics,
    float elapsed_time)
{
// 描画設定
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> immContext = p_graphics->GetImmContextPtr();

// ビュー・射影行列の設定
    m_pCameraController->SetMatrix(immContext);

// 描画用ライト設定
    //m_pLight->Set(immContext, m_pCameraController);
    m_pLightController->SetDataForGPU(immContext, m_pCameraController);

    m_pBlender->SetBlendState(immContext, Blender::BLEND_ALPHA);

// レンダーキュー内のモデル描画（MeshComponentは自動的にレンダーキューに登録）
    m_pRenderer->RenderQueue(immContext, elapsed_time);


// スプライトフォント
    //m_pBlender->SetBlendState(immContext, Blender::BLEND_ADD);
    static int timer = 0;

    m_pBlender->SetBlendState(immContext, Blender::BLEND_ADD);

    if (timer < 3600)
    {
        mFont->TextOutput(immContext, "Arrow key to Move", m_pSpriteShader, Vector2(16, 8), Vector2(16, 16), Vector4(1, 1, 1, 1));
        mFont->TextOutput(immContext, "Z key to Light Up", m_pSpriteShader, Vector2(16, 8 + 16), Vector2(16, 16), Vector4(1, 1, 1, 1));
    }
    timer++;


#ifdef _DEBUG
    m_pUIRenderer->RenderImGui(elapsed_time);
#endif
}

//----------------------------------------------------------------------------------------------------------------------------

Scene01::~Scene01()
{
}

//----------------------------------------------------------------------------------------------------------------------------

