#include "SceneB.h"
//#include "SceneManager.h"

#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/PointLight.h"
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/MeshRenderer.h"

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

    //
    // シェーダー初期化
    //
    m_pLambert = std::make_shared<Shader>();
    m_pLambert->createShader(
        device,
        L"./Src/Shaders/Lambert.hlsl",
        L"./Src/Shaders/Lambert.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT);

    m_pLambertForSkinnedMesh = std::make_shared<Shader>();
    m_pLambertForSkinnedMesh->createShader(
        device,
        L"./Src/Shaders/Lambert.hlsl",
        L"./Src/Shaders/Lambert.hlsl",
        "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);

    m_pLambertForSkinning = std::make_shared<Shader>();
    m_pLambertForSkinning->createShader(
        device,
        L"./Src/Shaders/Lambert.hlsl",
        L"./Src/Shaders/Lambert.hlsl",
        "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);

    m_pFromGBuffer = std::make_shared<Shader>();
    m_pFromGBuffer->createShader(
        device,
        L"./Src/Shaders/FromGBuffer.hlsl",
        L"./Src/Shaders/FromGBuffer.hlsl",
        "VSmain", "PSmain", VEDType::VED_SPRITE);

    m_pToGBuffer = std::make_shared<Shader>();
    m_pToGBuffer->createShader(
        device,
        L"./Src/Shaders/ToGBufferSimple.hlsl",
        L"./Src/Shaders/ToGBufferSimple.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT);

    m_pToGBufferForSkinnedMesh = std::make_shared<Shader>();
    m_pToGBufferForSkinnedMesh->createShader(
        device,
        L"./Src/Shaders/ToGBufferSimple.hlsl",
        L"./Src/Shaders/ToGBufferSimple.hlsl",
        "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);

    m_pToGBufferForSkinning = std::make_shared<Shader>();
    m_pToGBufferForSkinning->createShader(
        device,
        L"./Src/Shaders/ToGBufferSimple.hlsl",
        L"./Src/Shaders/ToGBufferSimple.hlsl",
        "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);

    m_pToGBufferForBump = std::make_shared<Shader>();
    m_pToGBufferForBump->createShader(
        device,
        L"./Src/Shaders/ToGBufferNormal.hlsl",
        L"./Src/Shaders/ToGBufferNormal.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT);

    m_pToGBufferSkinnedMeshForBump = std::make_shared<Shader>();
    m_pToGBufferSkinnedMeshForBump->createShader(
        device,
        L"./Src/Shaders/ToGBufferNormal.hlsl",
        L"./Src/Shaders/ToGBufferNormal.hlsl",
        "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);

    m_pDefferedLightShader = std::make_shared<Shader>();
    m_pDefferedLightShader->createShader(
        device,
        L"./Src/Shaders/DefferedPointLight.hlsl",
        L"./Src/Shaders/DefferedPointLight.hlsl",
        "VSmain", "PSmain", VEDType::VED_SPRITE);


    //
    // テクスチャ初期化
    //
    m_pMRT = std::make_unique<MultiRenderTarget>(device);
    m_pMRT->SetShader(m_pFromGBuffer);
    m_pMRT->SetShader(m_pDefferedLightShader);
    //m_pShadowMap = std::make_unique<ShadowMap>(device);


    m_pLightController->Init(4, 0);



// ----------------------------------------------------------------------------------------------
// アクター・コンポーネント作成
// ----------------------------------------------------------------------------------------------
    m_pPlayer = Actor::Initialize(ActorID::kPlayer);
    m_pPlayer->AddComponent(
        MeshComponent::MeshID::kBasicSphere,
        m_pRenderer,
        m_pToGBufferForBump,
        L"./Data/Images/earthmap.jpg"
    );
    m_pPlayer->SetScale(5, 5, 5);
    m_pPlayer->SetPosition(Vector3(0, 4, 0));
    m_pPlayer->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Images/earthnormal.jpg");
    m_pPlayer->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Images/earthbump.jpg");

    m_pActorManager->AddActor(m_pPlayer);

    m_pCat = Actor::Initialize(ActorID::kNonPlayer);
    m_pCat->AddComponent(
        MeshComponent::MeshID::kSkinnedMesh,
        m_pRenderer,
        m_pToGBufferForSkinnedMesh,
        "./Data/Models/oswell/cat_mdl.fbx"
    );

    m_pCat->SetPosition(Vector3(4, 1, 0));
    m_pCat->SetScale(0.5f, 0.5f, 0.5f);
    m_pCat->SetRotationQuaternion(-90.0f * 0.01745f, 0, 0);
    m_pActorManager->AddActor(m_pCat);

    m_pOswell = Actor::Initialize(ActorID::kEnemy);
    m_pOswell->AddComponent(
        MeshComponent::MeshID::kSkinnedMesh,
        m_pRenderer,
        m_pToGBufferForSkinning,
        "./Data/Models/oswell/oswell_test1.fbx"
    );

    m_pOswell->SetPosition(Vector3(-4, 3, 0));
    m_pOswell->SetScale(0.05f, 0.05f, 0.05f);
    m_pOswell->SetRotationQuaternion(-90.0f * 0.01745f, 0, 0);
    m_pOswell->GetComponent<MeshComponent>()->AddMotion("Move", "./Data/Models/oswell/oswell_movemove.fbx");
    m_pActorManager->AddActor(m_pOswell);

    m_pShiba = Actor::Initialize(ActorID::kPlane);
    m_pShiba->AddComponent(
        MeshComponent::MeshID::kSkinnedMesh,
        m_pRenderer,
        m_pToGBufferSkinnedMeshForBump,
        "./Data/Models/0810/shiba_mdl.fbx"
        );
    m_pShiba->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/0810/shina_N.png");
    m_pShiba->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/0810/shiba_H.png");

    m_pShiba->SetPosition(Vector3(0, 0.5f, -4));
    m_pShiba->SetRotationQuaternion(-90.0f * 0.01745f, 0, 0);
    m_pActorManager->AddActor(m_pShiba);

    m_pField = Actor::Initialize(ActorID::kFloor);
    m_pField->AddComponent(
        MeshComponent::MeshID::kStaticMesh,
        m_pRenderer,
        m_pToGBufferForBump,
        L"./Data/Models/OBJ/sea/sea.obj"
    );
    m_pField->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/OBJ/sea/Nsea.png");
    //m_pField->GetComponent<MeshComponent>()->addShaderResource(device, L"./Data/Models/OBJ/sea/Hsea.png");
    m_pField->SetPosition(Vector3(0, 0, 0));
    m_pField->SetScale(200, 200, 200);
    m_pActorManager->AddActor(m_pField);

    // 追跡カメラのターゲット設定
    m_pCameraController->SetTarget(m_pPlayer);


    //m_pRenderer->addShaderForShadow(m_pToShadowShader);
    //m_pOswell->GetComponent<MeshComponent>()->setShader(mpToShadowForMotion, true);
    //m_pCat->GetComponent<MeshComponent>()->setShader(mpToShadow, true);
}

void SceneB::InitializeScene()
{
}

void SceneB::Update(float elapsed_time)
{

    m_pCameraController->Update(elapsed_time);

    //m_pCamera->Update(elapsed_time);
    //m_pLight->Update(elapsed_time);
    m_pLightController->Update(elapsed_time);

    if (InputPtr->OnKeyDown("X"))
    {
        m_pOswell->GetComponent<MeshComponent>()->Play("Move");
    }

    m_pActorManager->Update(elapsed_time);

    static float angle = XM_PI / 4;
    angle += elapsed_time;
    float s = sinf(angle) * 5.0f;
    float c = cosf(angle) * 5.0f;
    m_pLightController->SetPointData(0, Vector3(-s, 3, c), Vector4(0.7f, 0.5f, 0.4f, 1.0f), 15.0f);
    m_pLightController->SetPointData(1, Vector3(-c, 3, s), Vector4(0.7f, 0.3f, 0.4f, 1.0f), 15.0f);
    m_pLightController->SetPointData(2, Vector3(s, 3, -c), Vector4(0.3f, 0.4f, 0.7f, 1.0f), 15.0f);
    m_pLightController->SetPointData(3, Vector3(c, 3, s), Vector4(0.7f, 0.3f, 0.3f, 1.0f), 15.0f);

}

void SceneB::Render(std::unique_ptr<GraphicsEngine>& p_graphics,
    float elapsed_time)
{
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> immContext = p_graphics->GetImmContextPtr();

    m_pCameraController->SetMatrix(immContext);

    m_pLightController->SetDataForGPU(immContext, m_pCameraController);

    m_pBlender->SetBlendState(immContext, Blender::BLEND_ALPHA);

    //m_pShadowMap->Activate(p_graphics, m_light);

    //m_pRenderer->renderShadowMap(immContext, elapsed_time, m_pShadowMap->getOrthoView());

    //m_pShadowMap->Deactivate(p_graphics);

    m_pMRT->ActivateGBuffer(p_graphics);

    m_pRenderer->RenderQueue(immContext, elapsed_time);

    //m_pMRT->Deactivate(p_graphics);

    m_pMRT->ActivateDefferedLight(p_graphics);

    m_pMRT->Deactivate(p_graphics);

    immContext->OMSetDepthStencilState(p_graphics->GetDepthStencilPtr(GraphicsEngine::DS_FALSE).Get(), 1);

    m_pMRT->Render(immContext, m_pBlender);



}

SceneB::~SceneB()
{
}
