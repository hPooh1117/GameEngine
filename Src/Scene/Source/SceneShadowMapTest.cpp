#include "SceneShadowMapTest.h"

#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Component/MeshComponent.h"

#include "./Engine/CameraController.h"
#include "./Engine/DirectionalLight.h"
#include "./Engine/GameSystem.h"
#include "./Engine/LightController.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/UIRenderer.h"

#include "./Renderer/ShadowMap.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Skybox.h"



//----------------------------------------------------------------------------------------------------------------------------

SceneF::SceneF(SceneManager* manager, D3D::DevicePtr& device) :Scene(manager, device)
{
	mNextScene = SceneID::SCENE_A;


	ENGINE.GetLightPtr()->Init(0, 0);
	ENGINE.CastShadow();
	ENGINE.SetIsDefferedRendering(false);
	ENGINE.SetSSAO(false);
	ENGINE.GetCameraPtr()->SetTarget(m_pPlayer);


	//----------------------------------------------------------------------------------------
	// アクター・コンポーネント初期化
	//----------------------------------------------------------------------------------------
	m_pPlayer = Actor::Initialize(ActorID::kPlayer);
	m_pPlayer->SetScale(0.05f, 0.05f, 0.05f);
	m_pPlayer->SetPosition(Vector3(0, 0.5f, 0));
	m_pPlayer->AddComponent<NewMeshComponent>();
	m_pPlayer->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_SkinnedMesh, ShaderType::EFromShadowForSkinning, L"./Data/Models/Female/Idle.fbx", FbxType::EMaya);
	m_pPlayer->GetComponent<NewMeshComponent>()->RegistMotion("Idle", L"./Data/Models/Female/Idle.fbx");
	m_pPlayer->GetComponent<NewMeshComponent>()->RegistMotion("Walking", L"./Data/Models/Female/Walking.fbx");
	m_pPlayer->GetComponent<NewMeshComponent>()->RegistMotion("Running", L"./Data/Models/Female/Running.fbx");
	m_pPlayer->GetComponent<NewMeshComponent>()->RegistAdditionalShader(ShaderType::EToShadowForSkinning, ShaderUsage::EShader);
	ENGINE.GetActorManagerPtr()->AddActor(m_pPlayer);

	m_pPlayer->GetComponent<NewMeshComponent>()->Play("Idle");

	m_pField = Actor::Initialize(ActorID::kNonPlayer);
	m_pField->SetPosition(Vector3(0, 0, 0));
	m_pField->SetScale(100.0f, 1.0f, 100.0f);
	m_pField->AddComponent<NewMeshComponent>();
	m_pField->GetComponent<NewMeshComponent>()->RegistMesh(MeshTypeID::E_BasicCube, ShaderType::EFromShadow, nullptr, FbxType::EDefault);
	m_pField->GetComponent<NewMeshComponent>()->RegistAdditionalShader(ShaderType::EToShadow, ShaderUsage::EShader);
	ENGINE.GetActorManagerPtr()->AddActor(m_pField);


}

void SceneF::InitializeScene()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneF::Update(float elapsed_time)
{
	if (InputPtr.OnKeyTrigger("2"))
	{
		m_pPlayer->GetComponent<NewMeshComponent>()->Play("Idle");
	}
	if (InputPtr.OnKeyTrigger("3"))
	{
		m_pPlayer->GetComponent<NewMeshComponent>()->Play("Walking");
	}
	if (InputPtr.OnKeyTrigger("4"))
	{
		m_pPlayer->GetComponent<NewMeshComponent>()->Play("Running");
	}
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneF::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
}

//----------------------------------------------------------------------------------------------------------------------------
