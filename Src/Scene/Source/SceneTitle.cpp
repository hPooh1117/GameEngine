#include "SceneTitle.h"
//#include "SceneManager.h"

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
#include "./Engine/GameSystem.h"

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

#include "./Renderer/Skybox.h"
#include "./Renderer/Blender.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Shader.h"
#include "./Renderer/sprite.h"



//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

SceneTitle::SceneTitle(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device) :Scene(manager, device)
{
    // 次のシーン設定
    mNextScene = SceneID::SCENE01;

    // ----------------------------------------------------------------------------------------------
    // プレファブ作成
    // ----------------------------------------------------------------------------------------------
        // スプライトフォント初期設定
    mFont = std::make_unique<Sprite>(device, L"./Data/Fonts/font0.png");

    //mSkyBox = std::make_unique<Skybox>(device, L"./Data/Images/canyon.jpg");

    // ライト初期設定
    //m_pLight = std::make_shared<CustomLight>(device);
    m_pLightController->Init(4, 4);

    // レンダーキュー初期設定
    //m_pRenderer = std::make_shared<MeshRenderer>(device, m_pCameraController, m_pLight);

    // 物理管理クラス初期設定
    m_pPhysicsManager = std::make_unique<PhysicsManager>();


    // ----------------------------------------------------------------------------------------------
    // シェーダー作成
    // ----------------------------------------------------------------------------------------------
    m_pLambert = std::make_shared<Shader>();
    m_pLambert->createShader(
        device,
        L"./Src/Shaders/BasicShader.hlsl",
        L"./Src/Shaders/BasicShader.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT
    );

    m_pPhongShader = std::make_shared<Shader>();
    m_pPhongShader->createShader(
        device,
        L"./Src/Shaders/CustomLight.hlsl",
        L"./Src/Shaders/CustomLight.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT);

    m_pPhongSkinned = std::make_shared<Shader>();
    m_pPhongShader->createShader(
        device,
        L"./Src/Shaders/CustomLight.hlsl",
        L"./Src/Shaders/CustomLight.hlsl",
        "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);

    m_pLineShader = std::make_shared<Shader>();
    m_pLineShader->createShader(
        device,
        L"./Src/Shaders/Line.hlsl",
        L"./Src/Shaders/Line.hlsl",
        "VSMainDuplicate", "PSmain", VEDType::VED_GEOMETRIC_PRIMITIVE
    );
    m_pLineShader->createGeometryShader(
        device,
        L"./Src/Shaders/Line.hlsl",
        "GSmainDuplicate"
    );

    m_pSpriteShader = std::make_shared<Shader>();
    m_pSpriteShader->createShader(
        device,
        L"./Src/Shaders/FromGBuffer.hlsl",
        L"./Src/Shaders/FromGBuffer.hlsl",
        "VSmain", "PSmain", VEDType::VED_SPRITE
    );


    // ----------------------------------------------------------------------------------------------
    // アクター・コンポーネント作成
    // ----------------------------------------------------------------------------------------------
    m_pPlayer = Actor::Initialize(ActorID::kPlayer);
    m_pPlayer->SetPosition(Vector3(0, 5, 0));
    m_pPlayer->SetScale(1.0f, 1.0f, 1.0f);
    m_pPlayer->AddComponent<CCPlayerMoveForPlanet>()->BeInvalid();
    m_pPlayer->AddComponent<CCPlayerLightController>()->SetConstantFlag(true);
    m_pPlayer->AddComponent(
        MeshComponent::MeshID::EBasicSphere,
        m_pRenderer,
        m_pPhongShader,
        L"\0"
    );
    //std::shared_ptr<MeshComponent> mesh = mpPlayer->AddComponent(
    //    MeshComponent::MeshID::EBasicCapsule,
    //    m_pRenderer,
    //    m_pLambert
    //    );
    m_pPlayer->AddComponent<CCCapsuleCollider>();

    //mesh->SetColliderData(mpPlayer->GetComponent<CCCapsuleCollider>());
    //mesh->SetWireframeMode();
    m_pPlayer->GetComponent<CCCapsuleCollider>()->SetOffset(Vector3(0, -0.4f, 0));
    m_pPlayer->GetComponent<CCCapsuleCollider>()->SetScale(Vector3(4.2f, 4.2f, 4.2f));
    m_pPlayer->AddComponent<CCPlayerCollector>();
    m_pActorManager->AddActor(m_pPlayer);

    //m_pEnemy = Actor::Initialize(ActorID::kEnemy);
    //m_pEnemy->AddComponent<MoveRotationComponent>();
    //m_pEnemy->AddComponent<CCSphereCollider>();
    //m_pEnemy->AddComponent(
    //    MeshComponent::MeshID::EBasicCube,
    //    m_pRenderer,
    //    m_pPhongShader,
    //    L"\0"
    //);
    //m_pEnemy->SetPosition(Vector3(5, 1, 0));
    //m_pEnemy->SetScale(1.0f, 1.0f, 1.0f);
    //m_pEnemy->GetComponent<MeshComponent>()->SetColor(Vector4(0.0f, 0.1f, 0.4f, 1.0f));
    //m_pActorManager->AddActor(m_pEnemy);

    //for (auto i = 0; i < 8; ++i)
    //{
    //    m_pTarget[i] = Actor::Initialize(ActorID::kNonPlayer0 + i);
    //    m_pTarget[i]->SetPosition(Vector3(25.0f * 0.5f * cosf(i * XM_PIDIV4 * 0.5f), 0/* + - i * 50.0f / 8.0f*/, 25.0f * 0.5f * sinf(i * XM_PIDIV4 * 0.5f)));
    //    m_pTarget[i]->SetScale(.025f, .025f, .025f);
    //    m_pTarget[i]->SetRotationQuaternion(90.0f * 0.001745f, 0, 0);
    //    m_pTarget[i]->AddComponent<CCPlanetResident>();
    //    m_pTarget[i]->AddComponent<CCTargetPulledUp>()->SetPlayer(mpPlayer);
    //    m_pTarget[i]->AddComponent<CCSphereCollider>()->ResetRadius(0.5f);
    //    m_pTarget[i]->AddComponent(
    //        MeshComponent::MeshID::ESkinnedMesh,
    //        m_pRenderer,
    //        m_pPhongShader,
    //        "./Data/Models/tank.fbx"
    //    );
    //    m_pTarget[i]->GetComponent<MeshComponent>()->SetColor(Vector4(0.5f, 0.1f, 0.1f, 1.0f));
    //    m_pActorManager->AddActor(m_pTarget[i]);

    //}
    m_pTarget = Actor::Initialize(ActorID::kNonPlayer);
    m_pTarget->SetPosition(Vector3(0, 1, 0));
    m_pTarget->SetScale(.01f, .01f, .01f);
    m_pTarget->SetRotationQuaternion(90.0f * 0.001745f, 0, 0);
    m_pTarget->AddComponent<MoveRotationComponent>();
    //m_pTarget->AddComponent<CCTargetPulledUp>()->SetPlayer(mpPlayer);
    m_pTarget->AddComponent<CCSphereCollider>()->ResetRadius(0.5f);
    m_pTarget->AddComponent(
        MeshComponent::MeshID::ESkinnedMesh,
        m_pRenderer,
        m_pPhongShader,
        "./Data/Models/tank.fbx"
    );
    m_pTarget->GetComponent<MeshComponent>()->SetColor(Vector4(0.5f, 0.1f, 0.1f, 1.0f));
    m_pActorManager->AddActor(m_pTarget);

    m_pField = Actor::Initialize(ActorID::kEnemy);
    m_pField->AddComponent(
        MeshComponent::MeshID::EBasicSphere,
        m_pRenderer,
        m_pPhongShader,
        L"\0"
    );
    //mpField->GetComponent<MeshComponent>()->SetWireframeMode();
    m_pField->SetScale(40.0f, 40.0f, 40.0f);
    m_pField->SetPosition(Vector3(0, -20.0f, 0));
    m_pField->AddComponent<StaticComponent>();
    m_pField->AddComponent<CCSphereCollider>()->ResetColliderType(ColliderType::kC_Planet);

    m_pActorManager->AddActor(m_pField);

//#ifdef _DEBUG
//    m_pGrid = Actor::Initialize(ActorID::EPlane);
//    m_pGrid->AddComponent(
//        MeshComponent::MeshID::EBasicLine,
//        m_pRenderer,
//        m_pLineShader
//    );
//    m_pGrid->SetScale(160.0f, 160.0f, 160.0f);
//    m_pGrid->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
//#endif

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
    //for (auto i = 0; i < 8; ++i)
    //{
    //    m_pTarget[i]->GetComponent<CCPlanetResident>()->RegistActorAsPlanet(mpField);
    //}
    //m_pTarget->GetComponent<CCPlanetResident>()->RegistActorAsPlanet(mpField);

    // UI描画クラスのキューに追加
#ifdef _DEBUG
    std::shared_ptr<UIClient> player(m_pPlayer->GetComponent<CCPlayerMoveForPlanet>());
    ENGINE.GetUIRenderer()->SetInQueue("Player Move", player);
    std::shared_ptr<UIClient> lightController(m_pPlayer->GetComponent<CCPlayerLightController>());
    ENGINE.GetUIRenderer()->SetInQueue("Player Light Controll", lightController);
    //std::shared_ptr<UIClient> target(m_pTarget->GetComponent<CCPlanetResident>());
    //m_pUIRenderer->SetInQueue("Target", target);
    //std::shared_ptr<UIClient> pulledUp(m_pTarget->GetComponent<CCTargetPulledUp>());
    //m_pUIRenderer->SetInQueue("Pulled Up", pulledUp);
    //std::shared_ptr<UIClient> light = std::static_pointer_cast<CustomLight>(m_pLight);
    //m_pUIRenderer->SetInQueue("Light", light);
    ENGINE.GetUIRenderer()->SetInQueue("Light", m_pLightController);

    std::shared_ptr<UIClient> camera = m_pCameraController;
    ENGINE.GetUIRenderer()->SetInQueue("Camera", camera);
    std::shared_ptr<UIClient> trace = std::static_pointer_cast<TraceCamera>(m_pCameraController->GetCameraPtr(1));
    ENGINE.GetUIRenderer()->SetInQueue("Trace", trace);
#endif
}

void SceneTitle::InitializeScene()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneTitle::Update(float elapsed_time)
{
    // 更新処理
    m_pCameraController->Update(elapsed_time);

    //for (auto i = 0; i < 8; ++i)
    //{
    //    m_pTarget[i]->GetComponent<MeshComponent>()->SetColor(Vector4((m_pTarget[i]->GetPosition().x + 20.0f) / 40.0f, (m_pTarget[i]->GetPosition().y + 20.0f) / 40.0f, (m_pTarget[i]->GetPosition().z + 20.0f) / 40.0f, 1.0f));
    //}
    static int timer = 0;
    if (timer > 100) timer -= 100;

    m_pTarget->GetComponent<MeshComponent>()->SetColor(Vector4(static_cast<float>(timer) / 100.0f, static_cast<float>(100 - timer) / 100.0f, static_cast<float>(timer) / 100.0f, 1.0f));
    timer++;

    //m_pLight->Update(elapsed_time);
    m_pLightController->Update(elapsed_time);

    m_pActorManager->Update(elapsed_time);

    m_pPhysicsManager->DetectCollision(m_pActorManager/*, elapsed_time*/);

    if (InputPtr.OnKeyTrigger("Space"))
    {
        mChangeFlag = true;
    }

}

//----------------------------------------------------------------------------------------------------------------------------

void SceneTitle::Render(
    std::unique_ptr<GraphicsEngine>& p_graphics,
    float elapsed_time)
{

    // 描画設定
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> immContext = p_graphics->GetImmContextPtr();

    // ビュー・射影行列の設定
    m_pCameraController->SetMatrix(immContext);

    // 描画用ライト設定
    //m_pLight->Set(immContext, m_pCameraController);
    m_pLightController->SetDataForGPU(immContext, m_pCameraController.get());

    m_pBlender->SetBlendState(immContext, Blender::BLEND_ALPHA);

    // レンダーキュー内のモデル描画（MeshComponentは自動的にレンダーキューに登録）
    m_pRenderer->RenderQueue(immContext, elapsed_time);


    // スプライトフォント
    m_pBlender->SetBlendState(immContext, Blender::BLEND_ADD);

    static int timer = 0;
    float alpha = static_cast<float>(timer / 8 % 8);
    //mFont->TextOutput(immContext, "Light Up The Tank", m_pSpriteShader, Vector2(SCREEN_WIDTH * 0.32f, SCREEN_HEIGHT * 0.5f), Vector2(32, 32), Vector4(1, 1, 1, 1));
    //mFont->TextOutput(immContext, "Push Space", m_pSpriteShader, Vector2(SCREEN_WIDTH * 0.4f, SCREEN_HEIGHT * 0.9f), Vector2(32, 32), Vector4(0.5f, 0.5f, 0.5f, alpha));
    ENGINE.GetUIRenderer()->SetText("Light Up The Tank");
    ENGINE.GetUIRenderer()->SetText("Push Space");

    timer++;
#ifdef _DEBUG
    ENGINE.GetUIRenderer()->SetNextWindowSettings(Vector2(0, 0), Vector2(300, 400));
    ENGINE.GetUIRenderer()->BeginRenderingNewWindow("Debugging");
    ENGINE.GetUIRenderer()->RenderUIQueue();
    ENGINE.GetUIRenderer()->FinishRenderingWindow();

#endif // _DEBUG
}

//----------------------------------------------------------------------------------------------------------------------------

