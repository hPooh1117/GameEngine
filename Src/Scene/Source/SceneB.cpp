#include "SceneB.h"
//#include "SceneManager.h"

#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Component/MeshComponent.h"

#include "./Engine/GameSystem.h"
#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
//#include "./Engine/MeshRenderer.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/ComputeExecuter.h"
#include "./Engine/Settings.h"

#include "./Renderer/Shader.h"
#include "./Renderer/Blender.h"
#include "./Renderer/ShadowMap.h"
#include "./Renderer/Renderer.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Skybox.h"


using namespace DirectX;

SceneB::SceneB(SceneManager * manager, Graphics::GraphicsDevice* p_device):Scene(manager, p_device)
{
    mNextScene = SceneID::SCENE_C;


    ENGINE.GetLightPtr()->Init(128, 0);
    ENGINE.GetLightPtr()->SetLightColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
    ENGINE.GetLightPtr()->SetShininess(30.0f);

    //ENGINE.GetLightPtr()->SetSpotData(0, Vector3(-5, 5, -5), Vector4(0.9f, 0.2f, 0.2f, 1.0f), Vector3(1.0f, -0.2f, 0.0f), 50.0f, 0.99f, 0.8f);
    //ENGINE.GetLightPtr()->SetSpotData(1, Vector3(-5, 5, 5), Vector4(0.2f, 0.2f, 0.9f, 1.0f), Vector3(-1.0f, -0.2f, 0.0f), 50.0f, 0.99f, 0.8f);
    //ENGINE.GetLightPtr()->SetSpotData(2, Vector3(5, 5, -5), Vector4(0.2f, 0.9f, 0.2f, 1.0f), Vector3(0.0f, -0.2f, 1.0f), 50.0f, 0.99f, 0.8f);
    //ENGINE.GetLightPtr()->SetSpotData(3, Vector3(5, 5, 5), Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector3(0.0f, -0.2f, -1.0f), 50.0f, 0.99f, 0.8f);
    //ENGINE.GetLightPtr()->SetPointData(0, Vector3(-5, 5, -5), Vector4(0.9f, 0.2f, 0.2f, 1.0f), 50.0f);
    //ENGINE.GetLightPtr()->SetPointData(1, Vector3(-5, 5, 5), Vector4(0.2f, 0.2f, 0.9f, 1.0f),   50.0f);
    //ENGINE.GetLightPtr()->SetPointData(2, Vector3(5, 5, -5), Vector4(0.2f, 0.9f, 0.2f, 1.0f),  50.0f);
    //ENGINE.GetLightPtr()->SetPointData(3, Vector3(5, 5, 5), Vector4(0.7f, 0.7f, 0.7f, 1.0f),    50.0f);


    Settings::Renderer renderSettings = {
        false,   // shadow
        false,  // ssao
        true,   // deffered
        false   // cubemap
    };
    ENGINE.SetRendererSettings(renderSettings);

// ----------------------------------------------------------------------------------------------
// アクター・コンポーネント作成
// ----------------------------------------------------------------------------------------------
    int count = 0;
    Actor* pPlayer = new Actor();
    pPlayer->SetScale(0.05f, 0.05f, 0.05f);
    pPlayer->SetPosition(Vector3(0, 0.5f, 0));
    pPlayer->AddComponent<MeshComponent>();
    pPlayer->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::EDefferedNormalForSkinning, L"./Data/Models/Female/Idle.fbx", FbxType::EMaya);
    pPlayer->GetComponent<MeshComponent>()->RegisterMotion("Idle", L"./Data/Models/Female/Idle.fbx");
    pPlayer->GetComponent<MeshComponent>()->RegisterMotion("Walking", L"./Data/Models/Female/Walking.fbx");
    pPlayer->GetComponent<MeshComponent>()->RegisterMotion("Running", L"./Data/Models/Female/Running.fbx");
    //pPlayer->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/Metal_Panel_Normal.jpg", TextureConfig::ENormalMap);
    //pPlayer->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/Metal_Panel_Height.png", TextureConfig::EHeightMap);
    //pPlayer->GetComponent<MeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadowForSkinning, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(pPlayer, count++);

    pPlayer->GetComponent<MeshComponent>()->Play("Idle");

    Actor* pField = new Actor();
    pField->SetPosition(Vector3(0, -0.5f, 0));
    pField->SetScale(200, 200, 200);
    pField->AddComponent<MeshComponent>();
    pField->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_StaticMesh, ShaderID::EDefferedSea, L"./Data/Models/OBJ/sea/sea.obj", FbxType::EDefault);
    pField->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Models/OBJ/sea/Nsea.png", TextureConfig::ENormalMap);
    pField->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Models/OBJ/sea/Hsea.png", TextureConfig::EHeightMap);
    //pField->GetComponent<MeshComponent>()->RegisterAdditionalShader(ShaderID::EToShadow, ShaderUsage::EShader);
    ENGINE.GetActorManagerPtr()->AddActor(pField, count++);


    ENGINE.GetCameraPtr()->SetTarget(pPlayer);
    ENGINE.GetCameraPtr()->SetPositionOfMoveableCamera(Vector3(0.0f, 10.0f, -60.0f));

}

void SceneB::InitializeScene()
{
}

void SceneB::Update(float elapsed_time)
{
    //if (InputPtr.OnKeyDown("X"))
    //{
    //    m_pOswell->GetComponent<MeshComponent>()->Play("Move");
    //}

    static float angle = XM_PI * 0.25f;
    angle += elapsed_time/* * 0.3f*/;

    float a = 16.0f * sinf(angle);
    float s = sinf(angle) * a;
    float c = cosf(angle) * a;
    float halfS = sinf(angle + XM_PI * 0.25f) * a;
    float halfC = cosf(angle + XM_PI * 0.25f) * a;
    float astS = sinf(angle + XM_PI * -0.25f) * a;
    float astC = cosf(angle + XM_PI * -0.25f) * a;
    float range = MathOp::Lerp(5.0f, 20.0f, abs(sinf(angle)));
    Vector4 R(0.9f, 0.20f, 0.05f, 1.0f);
    Vector4 G(0.2f, 0.99f, 0.3f, 1.0f);
    Vector4 B(0.1f, 0.4f, 0.99f, 1.0f);

    LightController* light = ENGINE.GetLightPtr().get();
    for (auto i = 0; i < 4; ++i)
    {
        light->SetPointData(0 + i * 16,  Vector3(s,      3, c),       R, range);
        light->SetPointData(1 + i * 16,  Vector3(-s,     3, c),       G, range);
        light->SetPointData(2 + i * 16,  Vector3(halfS,  3, halfC),   B, range);
        light->SetPointData(3 + i * 16,  Vector3(-halfS, 3, halfC),   R, range);
        light->SetPointData(4 + i * 16,  Vector3(s,      3, -c),      G, range);
        light->SetPointData(5 + i * 16,  Vector3(-s,     3, -c),      B, range);
        light->SetPointData(6 + i * 16,  Vector3(halfS,  3, -halfC),  R, range);
        light->SetPointData(7 + i * 16,  Vector3(-halfS, 3, -halfC),  G, range);
        light->SetPointData(8 + i * 16,  Vector3(astS,   3, astC),    B, range);
        light->SetPointData(9 + i * 16,  Vector3(-astS,  3, astC),    R, range);
        light->SetPointData(10 + i * 16, Vector3(astS,   3, -astC),   G, range);
        light->SetPointData(11 + i * 16, Vector3(-astS,  3, -astC),   B, range);
        light->SetPointData(12 + i * 16, Vector3(astC,   3, astS),    R, range);
        light->SetPointData(13 + i * 16, Vector3(-astC,  3, astS),    G, range);
        light->SetPointData(14 + i * 16, Vector3(astC,   3, -astS),   B, range);
        light->SetPointData(15 + i * 16, Vector3(-astC,  3, -astS),   R, range);



        s *= 1.5f; astS *= 1.5f; halfS *= 1.5f;
        c *= 1.5f; astC *= 1.5f; halfC *= 1.5f;
    }
}

void SceneB::PreCompute(Graphics::GraphicsDevice* p_graphics)
{
}

void SceneB::Render(Graphics::GraphicsDevice* p_graphics,
    float elapsed_time)
{
}

void SceneB::RenderUI()
{
    auto* ui = ENGINE.GetRenderer()->GetUIRenderer();
    Actor* player = ENGINE.GetActorManagerPtr()->GetActor(0);
    ui->SetNextWindowSettings(Vector2(0, SCREEN_HEIGHT - 200), Vector2(300, 200));
    ui->SetNextUIConfig(false);
    ui->BeginRenderingNewWindow("Motion");
    if (ImGui::Button("Idle"))    player->GetComponent<MeshComponent>()->Play("Idle");
    if (ImGui::Button("Walking")) player->GetComponent<MeshComponent>()->Play("Walking");
    if (ImGui::Button("Running")) player->GetComponent<MeshComponent>()->Play("Running");

    player->GetComponent<MeshComponent>()->RenderUI();



    ui->FinishRenderingWindow();

}

SceneB::~SceneB()
{
}
