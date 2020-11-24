#include "SceneShadowMapTest.h"

#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Component/MeshComponent.h"
#include "./Component/MoveRoundTrip.h"

#include "./Engine/CameraController.h"
#include "./Engine/DirectionalLight.h"
#include "./Engine/GameSystem.h"
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Settings.h"

#include "./Renderer/ShadowMap.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/RenderTarget.h"

#include "./RenderPass/RenderPasses.h"
#include "./RenderPass/MakeCubeMapPasses.h"

//----------------------------------------------------------------------------------------------------------------------------

SceneF::SceneF(SceneManager* manager, D3D::DevicePtr& device) :Scene(manager, device)
{
	mNextScene = SceneID::SCENE_A;


	ENGINE.GetLightPtr()->Init(0, 0);
	ENGINE.GetLightPtr()->SetLightColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

	ENGINE.GetCameraPtr()->SetTarget(m_pPlayer);

	Settings::Renderer renderSettings = {
	false,   // shadow
	false,  // ssao
	false,   // deffered
	true   // cubemap
	};
	ENGINE.SetRendererSettings(renderSettings);


	//----------------------------------------------------------------------------------------
	// アクター・コンポーネント初期化
	//----------------------------------------------------------------------------------------
	int count = 0;
	m_pPlayer = Actor::Initialize(count++);
	m_pPlayer->SetScale(0.05f, 0.05f, 0.05f);
	m_pPlayer->SetPosition(Vector3(0, 0.5f, 0));
	m_pPlayer->AddComponent<NewMeshComponent>();
	m_pPlayer->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::ESpotLightPhongForSkinning, L"./Data/Models/Female/Idle.fbx", FbxType::EMaya);
	m_pPlayer->GetComponent<NewMeshComponent>()->RegisterMotion("Idle", L"./Data/Models/Female/Idle.fbx");
	m_pPlayer->GetComponent<NewMeshComponent>()->RegisterMotion("Walking", L"./Data/Models/Female/Walking.fbx");
	m_pPlayer->GetComponent<NewMeshComponent>()->RegisterMotion("Running", L"./Data/Models/Female/Running.fbx");
	m_pPlayer->GetComponent<NewMeshComponent>()->RegisterAdditionalShader(ShaderID::EMakeCubeMapForSkinning, ShaderUsage::ECubeMap);
	ENGINE.GetActorManagerPtr()->AddActor(m_pPlayer);

	m_pPlayer->GetComponent<NewMeshComponent>()->Play("Walking");

	mpSphere = Actor::Initialize(count++);
	mpSphere->SetScale(5.0f, 5.0f, 5.0f);
	mpSphere->SetPosition(Vector3(0, 5.0f, 4.0f));
	mpSphere->AddComponent<MoveRoundTrip>();
	mpSphere->AddComponent<NewMeshComponent>();
	mpSphere->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EUseCubeMap, nullptr, FbxType::EDefault);
	mpSphere->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-tiles-01/Blue_tiles_01_2K_Base_Color.png", TextureConfig::EColorMap);
	ENGINE.GetActorManagerPtr()->AddActor(mpSphere);


	m_pField = Actor::Initialize(count++);
	m_pField->SetPosition(Vector3(0, 0, 0));
	m_pField->SetScale(100.0f, 0.1f, 100.0f);
	m_pField->AddComponent<NewMeshComponent>();
	m_pField->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::ESpotLightPhong, nullptr, FbxType::EDefault);
	m_pField->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/Test.png", TextureConfig::EColorMap);
	m_pField->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Models/OBJ/sea/Nsea.png", TextureConfig::ENormalMap);
	m_pField->GetComponent<NewMeshComponent>()->RegisterAdditionalShader(ShaderID::EMakeCubeMap, ShaderUsage::ECubeMap);

	ENGINE.GetActorManagerPtr()->AddActor(m_pField);


}

void SceneF::InitializeScene()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneF::Update(float elapsed_time)
{
	Vector3 pos = ENGINE.GetCameraPtr()->GetCameraPosition();
	pos.y = m_pField->GetPosition().y - pos.y;

	MakeCubeMapPass* pRenderPass = (MakeCubeMapPass*)ENGINE.GetRenderPass(RenderPassID::ECubeMapPass);
	//pRenderPass->SetOriginPoint(pos, 1.0f);
		pRenderPass->SetOriginPoint(mpSphere->GetPosition(), mpSphere->GetScaleValue() * 0.5f);

}

void SceneF::PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics)
{

}

//----------------------------------------------------------------------------------------------------------------------------

void SceneF::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	ENGINE.GetUIRenderer()->SetNextWindowSettings(Vector2(0, SCREEN_HEIGHT - 200), Vector2(300, 200));
	ENGINE.GetUIRenderer()->BeginRenderingNewWindow("Motion");
	if (ImGui::Button("Idle")) m_pPlayer->GetComponent<NewMeshComponent>()->Play("Idle");
	if (ImGui::Button("Walking")) m_pPlayer->GetComponent<NewMeshComponent>()->Play("Walking");
	if (ImGui::Button("Running")) m_pPlayer->GetComponent<NewMeshComponent>()->Play("Running");

	ENGINE.GetUIRenderer()->FinishRenderingWindow();
}

//----------------------------------------------------------------------------------------------------------------------------
