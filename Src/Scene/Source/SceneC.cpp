#include "SceneC.h"
//#include "SceneManager.h"

#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/PointLight.h"
#include "./Engine/SpotLight.h"
#include "./Engine/DirectionalLight.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/LightController.h"
#include "./Engine/MeshRenderer.h"

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

    //m_pPointLight = std::make_shared<Shader>();
    //m_pPointLight->createShader(
    //    device,
    //    L"./source/shader_source/FlatShaderGeo.hlsl",
    //    L"./source/shader_source/FlatShaderGeo.hlsl",
    //    "VSmain", "PSmain", VEDType::VED_DEFAULT);
    //m_pPointLight->createGeometryShader(
    //    device,
    //    L"./source/shader_source/FlatShaderGeo.hlsl",
    //    "GSmain"
    //);

    m_pPointLight = std::make_shared<Shader>();
    m_pPointLight->createShader(
        device,
        L"./Src/Shaders/PhongForSpotLight.hlsl",
        L"./Src/Shaders/PhongForSpotLight.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT);


    m_pPointLightForSkinnedMesh = std::make_shared<Shader>();
    m_pPointLightForSkinnedMesh->createShader(
        device,
        L"./Src/Shaders/PhongForSpotLight.hlsl",
        L"./Src/Shaders/PhongForSpotLight.hlsl",
        "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);

    //m_pFurShader = std::make_shared<Shader>();
    //m_pFurShader->createShader(
    //    device,
    //    L"./source/shader_source/FlatShaderGeo.hlsl",
    //    L"./source/shader_source/FlatShaderGeo.hlsl",
    //    "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);
    //m_pFurShader->createGeometryShader(
    //    device,
    //    L"./source/shader_source/FlatShaderGeo.hlsl",
    //    "GSmain"
    //);
    //m_pFurShader = std::make_shared<Shader>();
    //m_pFurShader->createShader(
    //    device,
    //    L"./source/shader_source/FurShaderGeo.hlsl",
    //    L"./source/shader_source/FurShaderGeo.hlsl",
    //    "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);
    //m_pFurShader->createGeometryShader(
    //    device,
    //    L"./source/shader_source/FurShaderGeo.hlsl",
    //    "GSmain"
    //);


    //m_pPointLightForSkinning = std::make_shared<Shader>();
    //m_pPointLightForSkinning->createShader(
    //    device,
    //    L"./source/shader_source/FlatShaderGeo.hlsl",
    //    L"./source/shader_source/FlatShaderGeo.hlsl",
    //    "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);
    //m_pPointLightForSkinning->createGeometryShader(
    //    device,
    //    L"./source/shader_source/FlatShaderGeo.hlsl",
    //    "GSmain"
    //);

    m_pPointLightForSkinning = std::make_shared<Shader>();
    m_pPointLightForSkinning->createShader(
        device,
        L"./Src/Shaders/PhongForSpotLight.hlsl",
        L"./Src/Shaders/PhongForSpotLight.hlsl",
        "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);


    m_pPointLightBump = std::make_shared<Shader>();
    m_pPointLightBump->createShader(
        device,
        L"./Src/Shaders/PhongForSpotLight.hlsl",
        L"./Src/Shaders/PhongForSpotLight.hlsl",
        "VSmainBump", "PSmainBump", VEDType::VED_DEFAULT);

    m_pPointLightForSkinnedMeshBump = std::make_shared<Shader>();
    m_pPointLightForSkinnedMeshBump->createShader(
        device,
        L"./Src/Shaders/PhongForSpotLight.hlsl",
        L"./Src/Shaders/PhongForSpotLight.hlsl",
        "VSmainSBump", "PSmainBump", VEDType::VED_SKINNED_MESH);

    m_pPointLightForOcean = std::make_shared<Shader>();
    m_pPointLightForOcean->createShader(
        device,
        L"./Src/Shaders/SpotLightSea.hlsl",
        L"./Src/Shaders/SpotLightSea.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT);

    //m_pFromGBuffer = std::make_shared<Shader>();
    //m_pFromGBuffer->createShader(
    //    device,
    //    L"./source/shader_source/FromGBuffer.hlsl",
    //    L"./source/shader_source/FromGBuffer.hlsl",
    //    "VSmain", "PSmain", VEDType::VED_SPRITE);

    //mSkyShader = std::make_shared<Shader>();
    //mSkyShader->createShader(
    //    device,
    //    L"./source/shader_source/SkyBox.hlsl",
    //    L"./source/shader_source/SkyBox.hlsl",
    //    "VSmain", "PSmain", VEDType::VED_DEFAULT
    //);

    m_pTesselation = std::make_shared<Shader>();
    m_pTesselation->createShader(
        device,
        L"./Src/Shaders/BasicTesselation1.hlsl",
        L"./Src/Shaders/BasicTesselation1.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT
    );
    m_pTesselation->createHullAndDomain(
        device,
        L"./Src/Shaders/BasicTesselation1.hlsl",
        "HSmain", "DSmain"
    );
    m_pTesselation->createGeometryShader(
        device,
        L"./Src/Shaders/BasicTesselation1.hlsl",
        "GSmain"
    );

    m_pTesselationForSkinnedMesh = std::make_shared<Shader>();
    m_pTesselationForSkinnedMesh->createShader(
        device,
        L"./Src/Shaders/BasicTesselation1.hlsl",
        L"./Src/Shaders/BasicTesselation1.hlsl",
        "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH
    );
    m_pTesselationForSkinnedMesh->createHullAndDomain(
        device,
        L"./Src/Shaders/BasicTesselation1.hlsl",
        "HSmain", "DSmain"
    );
    m_pTesselationForSkinnedMesh->createGeometryShader(
        device,
        L"./Src/Shaders/BasicTesselation1.hlsl",
        "GSmain"
    );

    m_pDisplacementMapping = std::make_shared<Shader>();
    m_pDisplacementMapping->createShader(
        device,
        L"./Src/Shaders/DisplacementMapping.hlsl",
        L"./Src/Shaders/DisplacementMapping.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT
    );
    m_pDisplacementMapping->createHullAndDomain(
        device,
        L"./Src/Shaders/DisplacementMapping.hlsl",
        "HSmain", "DSmain"
    );
    m_pDisplacementMapping->createGeometryShader(
        device,
        L"./Src/Shaders/DisplacementMapping.hlsl",
        "GSmain"
    );


    m_pLightController->Init(0, 4);
    m_pLightController->SetLightColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
    m_pLightController->SetSpotData(0, Vector3(-5, 5, -5), Vector4(0.9f, 0.2f, 0.2f, 1.0f), Vector3(1.0f, -0.2f, 0.0f), 50.0f, 0.99f, 0.8f);
    m_pLightController->SetSpotData(1, Vector3(-5, 5, 5), Vector4(0.2f, 0.2f, 0.9f, 1.0f), Vector3(-1.0f, -0.2f, 0.0f), 50.0f, 0.99f, 0.8f);
    m_pLightController->SetSpotData(2, Vector3(5, 5, -5), Vector4(0.2f, 0.9f, 0.2f, 1.0f), Vector3(0.0f, -0.2f, 1.0f), 50.0f, 0.99f, 0.8f);
    m_pLightController->SetSpotData(3, Vector3(5, 5, 5), Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector3(0.0f, -0.2f, -1.0f), 50.0f, 0.99f, 0.8f);




// ----------------------------------------------------------------------------------------------
// アクター・コンポーネント作成
// ----------------------------------------------------------------------------------------------
    m_pPlayer = Actor::Initialize(ActorID::kPlayer);
    m_pPlayer->AddComponent(
        MeshComponent::MeshID::kBasicSphere,
        m_pRenderer,
        m_pPointLight,
        L"./Data/Images/earthmap.jpg"
    );
    m_pPlayer->AddComponent<MoveRotationComponent>();
    m_pPlayer->SetScale(5, 5, 5);
    m_pPlayer->SetPosition(Vector3(0, 4, 0));
    m_pPlayer->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Images/earthnormal.jpg");
    m_pPlayer->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Images/earthbump.jpg");
    //m_pPlayer->GetComponent<MeshComponent>()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

    m_pActorManager->AddActor(m_pPlayer);

    m_pCat = Actor::Initialize(ActorID::kNonPlayer);
    m_pCat->AddComponent(
        MeshComponent::MeshID::kSkinnedMesh,
        m_pRenderer,
        m_pPointLightForSkinnedMesh,
        "./Data/Models/oswell/cat_mdl.fbx"
    );
    //m_pCat->GetComponent<MeshComponent>()->addShaderResource(device, L"./Data/Images/fur.png");
    m_pCat->SetPosition(Vector3(4, 1, 0));
    m_pCat->SetScale(0.5f, 0.5f, 0.5f);
    m_pCat->SetRotationQuaternion(-90.0f * 0.01745f, 0, 0);
    //m_pCat->GetComponent<MeshComponent>()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

    m_pActorManager->AddActor(m_pCat);

    m_pOswell = Actor::Initialize(ActorID::kEnemy);
    m_pOswell->AddComponent(
        MeshComponent::MeshID::kSkinnedMesh,
        m_pRenderer,
        m_pPointLightForSkinning,
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
        m_pPointLightForSkinnedMesh,
        "./Data/Models/0810/shiba_mdl.fbx"
    );
    m_pShiba->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/0810/shina_N.png");
    m_pShiba->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/0810/shiba_H.png");

    m_pShiba->SetPosition(Vector3(0, 0.5f, -4));
    m_pShiba->SetRotationQuaternion(-90.0f * 0.01745f, 0, 0);
    m_pActorManager->AddActor(m_pShiba);

    m_pSphere = Actor::Initialize(ActorID::kPlane);
    m_pSphere->AddComponent(
        MeshComponent::MeshID::kBasicSphere,
        m_pRenderer,
        m_pPointLight,
        L"\0"
    );
    m_pSphere->SetScale(5, 5, 5);
    m_pSphere->SetPosition(Vector3(0, 10, 0));
    //m_pSphere->GetComponent<MeshComponent>()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    m_pActorManager->AddActor(m_pSphere);

    m_pField = Actor::Initialize(ActorID::kFloor);
    m_pField->AddComponent(
        MeshComponent::MeshID::kStaticMesh,
        m_pRenderer,
        m_pPointLightForOcean,
        L"./Data/Models/OBJ/sea/sea.obj"
    );
    m_pField->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/OBJ/sea/Nsea.png");
    m_pField->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/OBJ/sea/Hsea.png");
    m_pField->SetPosition(Vector3(0, 0, 0));
    m_pField->SetScale(200, 200, 200);
    //m_pField->GetComponent<MeshComponent>()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

    m_pActorManager->AddActor(m_pField);

    //m_pSky = std::make_shared<Actor>(ActorID::kFloor);
    //m_pSky->AddComponent(
    //    MeshComponent::MeshID::kStaticMesh,
    //    m_pRenderer,
    //    mSkyShader,
    //    L"./Data/Models/OBJ/Sky/sky.obj"
    //);
    //m_pSky->setPosition(Vector3(0, 0, 0));
    //m_pSky->setScale(15, 15, 15);
    //m_pActorManager->AddActor(m_pField);

    //m_pRenderer->addShaderForShadow(m_pToShadowShader);
    //m_pOswell->GetComponent<MeshComponent>()->setShader(mpToShadowForMotion, true);
    //m_pCat->GetComponent<MeshComponent>()->setShader(mpToShadow, true);


// 追跡カメラのターゲット設定
    m_pCameraController->SetTarget(m_pSphere);

    //std::shared_ptr<UIClient> ui = std::static_pointer_cast<SpotLight>(m_pLight);
    //m_pUIRenderer = std::make_unique<UIRenderer>();
    //m_pUIRenderer->SetInQueue("SpotLight", ui);
    m_pUIRenderer->SetInQueue("Light", m_pLightController);
}

void SceneC::InitializeScene()
{
}

void SceneC::Update(float elapsed_time)
{
    m_pCameraController->Update(elapsed_time);

    m_pLightController->Update(elapsed_time);

    if (InputPtr->OnKeyDown("X"))
    {
        m_pOswell->GetComponent<MeshComponent>()->Play("Move");
    }




    m_pActorManager->Update(elapsed_time);
}

void SceneC::Render(std::unique_ptr<GraphicsEngine>& p_graphics,
    float elapsed_time)
{
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> immContext = p_graphics->GetImmContextPtr();

    m_pCameraController->SetMatrix(immContext);

    m_pLightController->SetDataForGPU(immContext, m_pCameraController);

    m_pBlender->SetBlendState(immContext, Blender::BLEND_ALPHA);




    m_pRenderer->RenderQueue(immContext, elapsed_time);
    m_pUIRenderer->RenderImGui(elapsed_time);


}

SceneC::~SceneC()
{
}
