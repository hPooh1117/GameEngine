#include "SceneD.h"

#include "./Component/MeshComponent.h"

#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/DirectionalLight.h"
#include "./Engine/GameSystem.h"
#include "./Engine/LightController.h"
#include "./Engine/MainCamera.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Settings.h"

#include "./Engine/AudioSystem.h"
#include "./Engine/PhysicsManager.h"
#include "./Engine/ParticleManager.h"

#include "./Component/MoveComponent.h"
#include "./Component/CCPlayerMove.h"
#include "./Component/CCPhysicalMove.h"
#include "./Component/SphereColliderComponent.h"
#include "./Component/MeshComponent.h"

#include "./Renderer/Sprite.h"
#include "./Renderer/Blender.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/Renderer.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Skybox.h"


//--------------------------------------------------------------------------------------------
// namespace
//--------------------------------------------------------------------------------------------
using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

SceneD::SceneD(SceneManager* manager, Graphics::GraphicsDevice* p_device)
	:Scene(manager, p_device)
{
	mNextScene = SceneID::SCENE_E;


	int count = 0;
	// ----------------------------------------------------------------------------------------------
	// �A�N�^�[�E�R���|�[�l���g�쐬
	// ----------------------------------------------------------------------------------------------
	Actor* pPlayer = new Actor();
	pPlayer->AddComponent<CCPlayerMove>();
	pPlayer->AddComponent<MeshComponent>();
	pPlayer->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::ELambert, nullptr, FbxType::EDefault);
	pPlayer->AddComponent<SphereColliderComponent>();
	pPlayer->SetPosition(Vector3(1.5f, 4, -3));
	pPlayer->GetComponent<MeshComponent>()->SetMaterialColor(Vector3(0.8f, 0.2f, 0.1f));
	ENGINE.GetActorManagerPtr()->AddActor(pPlayer, count++);

	for (int i = 0; i < 200; ++i)
	{
		Actor* actor = new Actor();
		actor->SetPosition(Vector3(
			static_cast<float>(rand() % 2000) / 100.0f - 10.0f,
			static_cast<float>(rand() % 2000) / 100.0f,
			static_cast<float>(rand() % 2000) / 100.0f - 10.0f));
		actor->SetScale(0.5f, 0.5f, 0.5f);
		actor->AddComponent<CCPhysicalMove>();
		actor->AddComponent<MeshComponent>();
		actor->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::ELambert, nullptr, FbxType::EDefault);
		actor->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/Sand_003_SD/Sand_003_COLOR.jpg", TextureConfig::EColorMap);
		actor->AddComponent<SphereColliderComponent>();
		ENGINE.GetActorManagerPtr()->AddActor(actor, count++);
	}
	// Field
	Actor* pField = new Actor();
	pField->AddComponent<MeshComponent>();
	pField->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::ELambert, nullptr, FbxType::EDefault);
	pField->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/Test.png", TextureConfig::EColorMap);
	pField->SetPosition(Vector3(0, -0.5f, 0));
	pField->SetScale(20.0f, 0.5f, 20.0f);
	ENGINE.GetActorManagerPtr()->AddActor(pField, count++);

	Actor* pUfo = new Actor();
	pUfo->SetPosition(Vector3(0, 3.0f, 0));
	pUfo->SetScale(0.1f, 0.1f, 0.1f);
	pUfo->AddComponent<CCPlayerMove>();
	pUfo->AddComponent<MeshComponent>();
	pUfo->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::ELambertSkinned, L"./Data/Models/ufooo/yh_g_walk.fbx", FbxType::EMaya);
	pUfo->GetComponent<MeshComponent>()->RegisterMotion("move", L"./Data/Models/ufooo/yh_g_walk.fbx");
	pUfo->GetComponent<MeshComponent>()->Play("move");
	pUfo->AddComponent<SphereColliderComponent>();
	ENGINE.GetActorManagerPtr()->AddActor(pUfo, count++);


	mpAudioSystem = std::make_unique<AudioSystem>();
	mpAudioSystem->loadMusic(0, L"./Data/Musics/Bird_Ambience.wav");
	mpAudioSystem->playMusic(0, true);

	mpPhysics = std::make_unique<PhysicsManager>();
	mpPhysics->Init(ENGINE.GetActorManagerPtr().get(), true);


	ENGINE.GetCameraPtr()->SetPositionOfMoveableCamera(Vector3(0, 5, -40));
	ENGINE.GetLightPtr()->Init(0, 0);


	Settings::Renderer renderSettings = {
		false,   // shadow
		false,  // ssao
		false,   // deffered
		false   // cubemap
	};
	ENGINE.SetRendererSettings(renderSettings);

	ENGINE.GetRenderer()->GetMeshRenderer()->SetSkybox(SkyboxTextureID::EFactoryCatWalk);

}

void SceneD::InitializeScene()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneD::Update(float elapsed_time)
{
	mpPhysics->DetectCollision(ENGINE.GetActorManagerPtr().get(), elapsed_time);

}

void SceneD::PreCompute(Graphics::GraphicsDevice* p_graphics)
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneD::Render(Graphics::GraphicsDevice* p_graphics,
	float elapsed_time)
{
}

void SceneD::RenderUI()
{
	auto* ui = ENGINE.GetRenderer()->GetUIRenderer();
	Actor* actor = ENGINE.GetActorManagerPtr()->GetActor(0);

	ui->SetText("player position :" +
		std::to_string(actor->GetPosition().x) + ". " +
		std::to_string(actor->GetPosition().y) + ". " +
		std::to_string(actor->GetPosition().z));
	ui->SetText("player velocity :" +
		std::to_string(actor->GetComponent<MoveComponent>()->GetVelocity().x) + ". " +
		std::to_string(actor->GetComponent<MoveComponent>()->GetVelocity().y) + ". " +
		std::to_string(actor->GetComponent<MoveComponent>()->GetVelocity().z));
	ui->SetText("collision counter :" +
		std::to_string(mpPhysics->GetCollisionCounter() / 2));
	ui->SetText("GamePad Connection : " +
		std::to_string(InputPtr.IsConnected(0)));

}

//----------------------------------------------------------------------------------------------------------------------------

SceneD::~SceneD()
{
}

//----------------------------------------------------------------------------------------------------------------------------
