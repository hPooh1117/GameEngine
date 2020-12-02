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
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Skybox.h"


//--------------------------------------------------------------------------------------------
// namespace
//--------------------------------------------------------------------------------------------
using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

SceneD::SceneD(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device)
	:Scene(manager, device)
{
	mNextScene = SceneID::SCENE_E;

	//mFont = std::make_unique<SpriteBatch>(device, L"./Data/Fonts/font0.png");



	//m_phong_shader = std::make_unique<Shader>();
	//m_phong_shader->createShader(
	//	device,
	//	L"./Src/Shaders/Lambert.hlsl",
	//	L"./Src/Shaders/Lambert.hlsl",
	//	"VSmain", "PSmain", VEDType::VED_DEFAULT
	//);



// ----------------------------------------------------------------------------------------------
// アクター・コンポーネント作成
// ----------------------------------------------------------------------------------------------
	mpPlayer = Actor::Initialize(ActorID::kPlayer);
	mpPlayer->AddComponent<CCPlayerMove>();
	//mpPlayer->AddComponent(
	//	MeshComponent::MeshID::EBasicSphere,
	//	m_pRenderer,
	//	m_phong_shader,
	//	L"./Data/Images/check.jpg");
	mpPlayer->AddComponent<NewMeshComponent>();
	mpPlayer->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::ELambert, nullptr, FbxType::EDefault);
	mpPlayer->AddComponent<SphereColliderComponent>();
	mpPlayer->SetPosition(Vector3(1.5f, 4, -3));
	mpPlayer->GetComponent<NewMeshComponent>()->SetMaterialColor(Vector3(0.8f, 0.2f, 0.1f));
	ENGINE.GetActorManagerPtr()->AddActor(mpPlayer);

	int actorCounter = 1;
	for (auto& actor : mpActors)
	{
		actor = Actor::Initialize(actorCounter++);
		actor->SetPosition(Vector3(
			static_cast<float>(rand() % 2000) / 100.0f - 10.0f,
			static_cast<float>(rand() % 2000) / 100.0f,
			static_cast<float>(rand() % 2000) / 100.0f - 10.0f));
		actor->SetScale(0.5f, 0.5f, 0.5f);
		actor->AddComponent<CCPhysicalMove>();
		//actor->AddComponent(
		//	MeshComponent::MeshID::EBasicSphere,
		//	m_pRenderer,
		//	m_phong_shader,
		//	L"./Data/Images/Sand_003_SD/Sand_003_COLOR.jpg"
		//	);
		actor->AddComponent<NewMeshComponent>();
		actor->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::ELambert, nullptr, FbxType::EDefault);
		actor->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/Sand_003_SD/Sand_003_COLOR.jpg", TextureConfig::EColorMap);
		actor->AddComponent<SphereColliderComponent>();
		ENGINE.GetActorManagerPtr()->AddActor(actor);
	}
	// Field
	mpField = Actor::Initialize(actorCounter);
	//mpField->AddComponent(
	//	MeshComponent::MeshID::EBasicCube,
	//	m_pRenderer,
	//	m_phong_shader,
	//	L"./Data/Images/Test.png"
	//);
	mpField->AddComponent<NewMeshComponent>();
	mpField->GetComponent<NewMeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::ELambert, nullptr, FbxType::EDefault);
	mpField->GetComponent<NewMeshComponent>()->RegisterTexture(L"./Data/Images/Test.png", TextureConfig::EColorMap);
	mpField->SetPosition(Vector3(0, -0.5f, 0));
	mpField->SetScale(20.0f, 0.5f, 20.0f);
	ENGINE.GetActorManagerPtr()->AddActor(mpField);


	mpAudioSystem = std::make_unique<AudioSystem>();
	mpAudioSystem->loadMusic(0, L"./Data/Musics/Bird_Ambience.wav");
	mpAudioSystem->playMusic(0, true);

	mpPhysics = std::make_unique<PhysicsManager>();
	mpPhysics->Init(ENGINE.GetActorManagerPtr(), true);


	ENGINE.GetCameraPtr()->SetPositionOfMoveableCamera(Vector3(0, 5, -40));
	ENGINE.GetLightPtr()->Init(0, 0);


	Settings::Renderer renderSettings = {
		false,   // shadow
		false,  // ssao
		false,   // deffered
		false   // cubemap
	};
	ENGINE.SetRendererSettings(renderSettings);

}

void SceneD::InitializeScene()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneD::Update(float elapsed_time)
{
	//m_pCameraController->Update(elapsed_time);

	//m_pLightController->Update(elapsed_time);

	ENGINE.GetActorManagerPtr()->Update(elapsed_time);

	mpPhysics->DetectCollision(ENGINE.GetActorManagerPtr(), elapsed_time);

}

void SceneD::PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics)
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneD::Render(std::unique_ptr<GraphicsEngine>& p_graphics,
	float elapsed_time)
{
}

void SceneD::RenderUI()
{
	ENGINE.GetUIRenderer()->SetText("player position :" +
		std::to_string(mpPlayer->GetPosition().x) + ". " +
		std::to_string(mpPlayer->GetPosition().y) + ". " +
		std::to_string(mpPlayer->GetPosition().z));
	ENGINE.GetUIRenderer()->SetText("player velocity :" +
		std::to_string(mpPlayer->GetComponent<MoveComponent>()->GetVelocity().x) + ". " +
		std::to_string(mpPlayer->GetComponent<MoveComponent>()->GetVelocity().y) + ". " +
		std::to_string(mpPlayer->GetComponent<MoveComponent>()->GetVelocity().z));
	ENGINE.GetUIRenderer()->SetText("collision counter :" +
		std::to_string(mpPhysics->GetCollisionCounter() / 2));
	ENGINE.GetUIRenderer()->SetText("GamePad Connection : " +
		std::to_string(InputPtr.IsConnected(0)));

}

//----------------------------------------------------------------------------------------------------------------------------

SceneD::~SceneD()
{
}

//----------------------------------------------------------------------------------------------------------------------------
