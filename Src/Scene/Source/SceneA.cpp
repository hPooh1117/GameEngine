#include "SceneA.h"
#include "SceneManager.h"

#include "./Application/Application.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/DirectionalLight.h"
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/MeshRenderer.h"

#include "./Component/MoveRotationComponent.h"

#include "./Renderer/Skybox.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/sprite.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"



using namespace DirectX;

SceneA::SceneA(SceneManager * manager, Microsoft::WRL::ComPtr<ID3D11Device>& device):Scene(manager, device)
{
    mNextScene = SceneID::SCENE_B;

    // Initialize sprite font
    mFont = std::make_unique<Sprite>(device, L"./Data/Fonts/font0.png");

    m_pSkyBox = std::make_unique<SkyBox>(device, L"./Data/Images/canyon.jpg");

    m_pLightController->Init(0, 0, true);



    m_pPhong = std::make_shared<Shader>();
    m_pPhong->createShader(
        device,
        L"./Src/Shaders/Phong.hlsl",
        L"./Src/Shaders/Phong.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT
    );
    m_line_shader = std::make_shared<Shader>();
    m_line_shader->createShader(
        device,
        L"./Src/Shaders/Line.hlsl",
        L"./Src/Shaders/Line.hlsl",
        "VSMainDuplicate", "PSmain", VEDType::VED_GEOMETRIC_PRIMITIVE
    );
    m_line_shader->createGeometryShader(
        device,
        L"./Src/Shaders/Line.hlsl",
        "GSmainDuplicate"
    );

    m_env_map_shader = std::make_shared<Shader>();
    m_env_map_shader->createShader(
        device,
        L"./Src/Shaders/EnvironmentMap.hlsl",
        L"./Src/Shaders/EnvironmentMap.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT
    );
    m_sky_shader = std::make_shared<Shader>();
    m_sky_shader->createShader(
        device,
        L"./Src/Shaders/SkyBox.hlsl",
        L"./Src/Shaders/SkyBox.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT
    );
    m_bump_shader = std::make_shared<Shader>();
    m_bump_shader->createShader(
        device,
        L"./Src/Shaders/NormalMap.hlsl",
        L"./Src/Shaders/NormalMap.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT
    );
    m_toon_shader = std::make_shared<Shader>();
    m_toon_shader->createShader(
        device,
        L"./Src/Shaders/ToonGeo.hlsl",
        L"./Src/Shaders/ToonGeo.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT
    );
    m_toon_shader->createGeometryShader(
        device,
        L"./Src/Shaders/ToonGeo.hlsl",
        "GSmain"
    );
    m_pSpriteShader = std::make_shared<Shader>();
    m_pSpriteShader->createShader(
        device,
        L"./Src/Shaders/FromGBuffer.hlsl",
        L"./Src/Shaders/FromGBuffer.hlsl",
        "VSmain", "PSmain", VEDType::VED_SPRITE
    );

    m_pForwardBRDF = std::make_shared<Shader>();
    m_pForwardBRDF->createShader(
        device,
        L"./Src/Shaders/ForwardPBR.hlsl",
        L"./Src/Shaders/ForwardPBR.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT
    );

// ----------------------------------------------------------------------------------------------
// アクター・コンポーネント作成
// ----------------------------------------------------------------------------------------------
    m_pSphere = Actor::Initialize(ActorID::kPlayer);
    m_pSphere->AddComponent<MoveRotationComponent>();
    m_pSphere->AddComponent(
        MeshComponent::MeshID::kBasicSphere,
        m_pRenderer,
        m_env_map_shader,
        L"./Data/Images/check.jpg"
    );
    m_pSphere->GetComponent<MeshComponent>()->AddShaderResource
    (
        device,
        L"./Data/Images/canyon.jpg"
    );
    m_pSphere->SetScale(5.0f, 5.0f, 5.0f);
    m_pActorManager->AddActor(m_pSphere);

    m_pEarth = Actor::Initialize(ActorID::kNonPlayer);
    m_pEarth->AddComponent<MoveRotationComponent>();
    m_pEarth->AddComponent(
        MeshComponent::MeshID::kBasicSphere,
        m_pRenderer,
        m_bump_shader,
        L"./Data/Images/earthmap.jpg"
    );
    m_pEarth->SetPosition(Vector3(5, 1, 0));
    m_pEarth->SetScale(5.0f, 5.0f, 5.0f);

    m_pEarth->GetComponent<MeshComponent>()->AddShaderResource(device,
        L"./Data/Images/earthnormal.jpg");
    m_pEarth->GetComponent<MeshComponent>()->AddShaderResource(device,
        L"./Data/Images/earthbump.jpg");
    m_pActorManager->AddActor(m_pEarth);


    m_pToon = Actor::Initialize(ActorID::kEnemy);
    m_pToon->AddComponent<MoveRotationComponent>();
    m_pToon->AddComponent(
        MeshComponent::MeshID::kBasicSphere,
        m_pRenderer,
        m_toon_shader,
        L"./Data/Images/Test.png"
    );
    m_pToon->SetPosition(Vector3(-5, 1, 0));
    m_pToon->SetScale(5, 5, 5);
    m_pActorManager->AddActor(m_pToon);

    m_pPBRTest = Actor::Initialize(ActorID::kFloor);
    m_pPBRTest->AddComponent<MoveRotationComponent>();
    m_pPBRTest->AddComponent(
        MeshComponent::MeshID::kBasicSphere,
        m_pRenderer,
        m_pForwardBRDF,
        L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Base_Color.png"
    );
    m_pPBRTest->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Images/canyon.jpg");
    m_pPBRTest->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_3K_Metallic.png");
    m_pPBRTest->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Roughness.png");
    m_pPBRTest->SetPosition(Vector3(0, 6, 0));
    m_pPBRTest->SetScale(5, 5, 5);
    m_pPBRTest->GetComponent<MeshComponent>()->SetColor(Vector4(0.8f, 0.6f, 0.3f, 1.0f));
    m_pActorManager->AddActor(m_pPBRTest);

    

    mGrid = Actor::Initialize(ActorID::kPlane);
    mGrid->AddComponent(
        MeshComponent::MeshID::kBasicLine,
        m_pRenderer,
        m_line_shader
    );
    mGrid->SetScale(160.0f, 160.0f, 160.0f);
    mGrid->SetPosition(Vector3(0.0f, 0.0f, 0.0f));


// ----------------------------------------------------------------------------------------------
// 追加設定
// ----------------------------------------------------------------------------------------------
    // 追跡カメラのターゲット設定
    m_pCameraController->SetTarget(m_pSphere);

    m_pUIRenderer->SetInQueue("Light", m_pLightController);
}

void SceneA::Update(float elapsed_time)
{
    m_pCameraController->Update(elapsed_time);

    m_pLightController->Update(elapsed_time);

    m_pActorManager->Update(elapsed_time);

}

void SceneA::Render(
    std::unique_ptr<GraphicsEngine>& p_graphics,
    float elapsed_time)
{
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> immContext = p_graphics->GetImmContextPtr();

    // ビュー・射影行列の設定
    m_pCameraController->SetMatrix(immContext);

    //m_pLight->Set(immContext, m_pCameraController);
    m_pLightController->SetDataForGPU(immContext, m_pCameraController);

    m_pBlender->SetBlendState(immContext, Blender::BLEND_ALPHA);

    XMMATRIX w = XMMatrixIdentity();
    m_pSkyBox->Render(immContext, elapsed_time, w, m_pCameraController, m_sky_shader);

    m_pRenderer->RenderQueue(immContext, elapsed_time);


    m_pBlender->SetBlendState(immContext, Blender::BLEND_ADD);
    mFont->TextOutput(immContext, "mIsSolid =" + std::to_string(mIsSolid), m_pSpriteShader, Vector2(16, 8), Vector2(16, 16), Vector4(1, 1, 1, 1));

    m_pBlender->SetBlendState(immContext, Blender::BLEND_ALPHA);

    m_pUIRenderer->RenderImGui(elapsed_time);
}


SceneA::~SceneA()
{
}


