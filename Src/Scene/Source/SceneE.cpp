#include "SceneE.h"
#include "SceneManager.h"

#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/DirectionalLight.h" // TODO: Will be Deleted
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/MeshRenderer.h"


#include "./Component/MoveRotationComponent.h"

#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Blender.h"
#include "./Renderer/ShadowMap.h"
#include "./Renderer/AmbientOcculusion.h"
#include "./Renderer/MultiRenderTarget.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Skybox.h"


//----------------------------------------------------------------------------------------------------------------------------

SceneE::SceneE(SceneManager* manager, D3D::DevicePtr& device) :Scene(manager, device)
{
	mNextScene = SceneID::SCENE_F;

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
		"VSmain2", "PSmainEx", VEDType::VED_DEFAULT);

	m_pToShadow = std::make_shared<Shader>();
	m_pToShadow->createShader(
		device,
        L"./Src/Shaders/ShadowMap.hlsl",
        L"./Src/Shaders/ShadowMap.hlsl",
        "VSshadowS", "PSshadow", VEDType::VED_SKINNED_MESH);
	m_pFromShadow = std::make_shared<Shader>();
	m_pFromShadow->createShader(
		device,
        L"./Src/Shaders/ShadowMap.hlsl",
        L"./Src/Shaders/ShadowMap.hlsl",
        "VSmainS2", "PSmainEx", VEDType::VED_SKINNED_MESH);

	m_pToShadowForMotion = std::make_shared<Shader>();
	m_pToShadowForMotion->createShader(
		device,
        L"./Src/Shaders/ShadowMap.hlsl",
        L"./Src/Shaders/ShadowMap.hlsl",
        "VSshadowSkinning", "PSshadow", VEDType::VED_SKINNED_MESH);
	m_pFromShadowForMotion = std::make_shared<Shader>();
	m_pFromShadowForMotion->createShader(
		device,
        L"./Src/Shaders/ShadowMap.hlsl",
        L"./Src/Shaders/ShadowMap.hlsl",
        "VSmainSkinning", "PSmainEx", VEDType::VED_SKINNED_MESH);

  
//--------------------------------------------------------------------------------
// DEFFERED
//--------------------------------------------------------------------------------
    m_pFromGBuffer = std::make_shared<Shader>();
    m_pFromGBuffer->createShader(
        device,
        L"./Src/Shaders/FromGBuffer.hlsl",
        L"./Src/Shaders/FromGBuffer.hlsl",
        "VSmain", "PSmain", VEDType::VED_SPRITE);

    m_pToGBuffer = std::make_shared<Shader>();
    m_pToGBuffer->createShader(
        device,
        L"./Src/Shaders/ToGBufferSSAO.hlsl",
        L"./Src/Shaders/ToGBufferSSAO.hlsl",
        "VSmain", "PSmain", VEDType::VED_DEFAULT);

    m_pToGBufferForSkinnedMesh = std::make_shared<Shader>();
    m_pToGBufferForSkinnedMesh->createShader(
        device,
        L"./Src/Shaders/ToGBufferSSAO.hlsl",
        L"./Src/Shaders/ToGBufferSSAO.hlsl",
        "VSmainS", "PSmain", VEDType::VED_SKINNED_MESH);

    m_pToGBufferForSkinning = std::make_shared<Shader>();
    m_pToGBufferForSkinning->createShader(
        device,
        L"./Src/Shaders/ToGBufferSSAO.hlsl",
        L"./Src/Shaders/ToGBufferSSAO.hlsl",
        "VSmainSkinning", "PSmain", VEDType::VED_SKINNED_MESH);


    m_pDefferedLightShader = std::make_shared<Shader>();
    m_pDefferedLightShader->createShader(
        device,
        L"./Src/Shaders/SSAO.hlsl",
        L"./Src/Shaders/SSAO.hlsl",
        "VSmain", "PSmain", VEDType::VED_SPRITE);


//----------------------------------------------------------------------------------------
// テクスチャ初期化
//----------------------------------------------------------------------------------------
    m_pMRT = std::make_shared<MultiRenderTarget>(device);
    m_pMRT->SetShader(m_pFromGBuffer);
    m_pMRT->SetShader(m_pDefferedLightShader);
    m_pShadowMap = std::make_unique<ShadowMap>(device);
    m_pAO = std::make_unique<AmbientOcclusion>(device);

//----------------------------------------------------------------------------------------
// プレファブ初期化
//----------------------------------------------------------------------------------------

    m_pLightController->Init(0, 0);



//----------------------------------------------------------------------------------------
// アクター・コンポーネント初期化
//----------------------------------------------------------------------------------------
    m_pCat = Actor::Initialize(ActorID::kNonPlayer);
    m_pCat->AddComponent(
        MeshComponent::MeshID::kBasicCylinder,
        m_pRenderer,
        m_pToGBuffer
    );

    m_pCat->SetPosition(Vector3(1.0f, 1, -1.5f - 5.0f));
    //m_pCat->SetScale(0.5f, 0.5f, 0.5f);
    m_pCat->SetScale(2, 4, 2);
    //m_pCat->SetAdditiveRotation(-90.0f, 0.0f, 0.0f);
    m_pCat->SetRotationQuaternion(0, 0, 0);
    m_pActorManager->AddActor(m_pCat);

    m_pOswell = Actor::Initialize(ActorID::kEnemy);
    m_pOswell->AddComponent(
        MeshComponent::MeshID::kSkinnedMesh,
        m_pRenderer,
        m_pToGBufferForSkinning,
        "./Data/Models/oswell/oswell_test1.fbx"
    );

    //m_pOswell->SetPosition(Vector3(-4, 3, 0));
    m_pOswell->SetPosition(Vector3(-2.5f, 2.5f, 0.0f - 5.0f));
    m_pOswell->SetScale(0.05f, 0.05f, 0.05f);
    //m_pOswell->SetScale(5, 5, 5);
    m_pOswell->SetAdditiveRotation(-90.0f, 0.0f, 0.0f);
    m_pOswell->SetRotationQuaternion(0, 0, 0);
    //m_pOswell->AddComponent<MoveRotationComponent>();
    //m_pOswell->GetComponent<MeshComponent>()->AddMotion("Move", "./Data/Models/oswell/oswell_movemove.fbx");
    m_pActorManager->AddActor(m_pOswell);

    m_pShiba = Actor::Initialize(ActorID::kPlane);
    m_pShiba->AddComponent(
        MeshComponent::MeshID::kBasicSphere,
        m_pRenderer,
        m_pToGBuffer,
        L"./Data/Images/PBR/cgbookcase/painted-metal-02/Painted_metal_02_2K_Base_Color.png"
    );
    //m_pShiba->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/0810/shina_N.png");
    //m_pShiba->GetComponent<MeshComponent>()->AddShaderResource(device, L"./Data/Models/0810/shiba_H.png");
    m_pShiba->SetAdditiveRotation(-90.0f, 0.0f, 0.0f);
    m_pShiba->SetScale(3, 3, 3);
    m_pShiba->SetPosition(Vector3(0, 1.5f, -4 - 4.0f));
    m_pShiba->SetRotationQuaternion(0, 0, 0);
    m_pActorManager->AddActor(m_pShiba);

    m_pField = Actor::Initialize(ActorID::kFloor);
    m_pField->AddComponent(
        MeshComponent::MeshID::kStaticMesh,
        m_pRenderer,
        m_pToGBuffer,
        L"./Data/Models/OBJ/Field/field.obj"
    );
    m_pField->SetPosition(Vector3(0, 0, 0));
    m_pField->SetScale(10, 10, 10);
    //m_pField->GetComponent<MeshComponent>()->SetWireframeMode();
    m_pActorManager->AddActor(m_pField);


    // 追跡カメラのターゲット設定
    m_pCameraController->SetTarget(m_pShiba);

    m_pRenderer->AddShaderForShadow(m_pToShadowShader);
    //m_pOswell->GetComponent<MeshComponent>()->SetShader(m_pToShadowForMotion, true);
    //m_pCat->GetComponent<MeshComponent>()->SetShader(m_pToShadow, true);
    //m_pShiba->GetComponent<MeshComponent>()->SetShader(m_pToShadow, true);

    m_pUIRenderer->SetInQueue("GBuffer", std::static_pointer_cast<UIClient>(m_pMRT));
    m_pUIRenderer->SetInQueue("Light", m_pLightController);
    m_pUIRenderer->SetInQueue("Camera", m_pCameraController);
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneE::Update(float elapsed_time)
{

    m_pCameraController->Update(elapsed_time);

    //m_pLight->Update(elapsed_time);
    m_pLightController->Update(elapsed_time);

    if (InputPtr->OnKeyDown("X"))
    {
        m_pOswell->GetComponent<MeshComponent>()->Play("Move");
    }

    m_pActorManager->Update(elapsed_time);

    
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneE::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
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
// GBuffer用意
//---------------------------------------------------------------------------------------------
    m_pMRT->ActivateGBuffer(p_graphics);

    m_pCameraController->SetMatrix(immContext);

    m_pLightController->SetDataForGPU(immContext, m_pCameraController);

    m_pAO->Activate(p_graphics, m_pCameraController);

    m_pRenderer->RenderQueue(immContext, elapsed_time);

//---------------------------------------------------------------------------------------------
// PreLighting
//---------------------------------------------------------------------------------------------
    m_pMRT->ActivateDefferedLight(p_graphics);

    m_pMRT->Deactivate(p_graphics);

    immContext->OMSetDepthStencilState(p_graphics->GetDepthStencilPtr(GraphicsEngine::DS_FALSE).Get(), 1);

//---------------------------------------------------------------------------------------------
// 描画処理
//---------------------------------------------------------------------------------------------
    m_pMRT->Render(immContext, m_pBlender);


    m_pUIRenderer->RenderImGui(elapsed_time);
}

//----------------------------------------------------------------------------------------------------------------------------

SceneE::~SceneE()
{
}

//----------------------------------------------------------------------------------------------------------------------------
