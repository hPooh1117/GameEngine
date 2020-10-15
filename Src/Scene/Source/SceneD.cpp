//--------------------------------------------------------------------------------------------
// include
//--------------------------------------------------------------------------------------------
#include "SceneD.h"

#include "./Engine/DirectionalLight.h"
#include "./Engine/LightController.h"
#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/ActorManager.h"
#include "./Engine/MeshRenderer.h"

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

	mFont = std::make_unique<SpriteBatch>(device, L"./Data/Fonts/font0.png");

	m_pLightController->Init(0, 0);


	m_phong_shader = std::make_unique<Shader>();
	m_phong_shader->createShader(
		device,
		L"./Src/Shaders/Lambert.hlsl",
		L"./Src/Shaders/Lambert.hlsl",
		"VSmain", "PSmain", VEDType::VED_DEFAULT
	);


	m_pCameraController->SetPositionOfMoveableCamera(Vector3(0, 5, -40));

// ----------------------------------------------------------------------------------------------
// アクター・コンポーネント作成
// ----------------------------------------------------------------------------------------------
	m_player = Actor::Initialize(ActorID::kPlayer);
	m_player->AddComponent<CCPlayerMove>();
	m_player->AddComponent(
		MeshComponent::MeshID::kBasicSphere,
		m_pRenderer,
		m_phong_shader,
		L"./Data/Images/check.jpg");
	m_player->AddComponent<SphereColliderComponent>();
	m_player->SetPosition(Vector3(1.5f, 4, -3));
	m_player->GetComponent<MeshComponent>()->SetColor(Vector4(1, 0, 0, 1));
	m_pActorManager->AddActor(m_player);

	int actorCounter = 1;
	for (auto& actor : m_actors)
	{
		actor = Actor::Initialize(actorCounter++);
		actor->SetPosition(Vector3(
			static_cast<float>(rand() % 2000) / 100.0f - 10.0f,
			static_cast<float>(rand() % 2000) / 100.0f,
			static_cast<float>(rand() % 2000) / 100.0f - 10.0f));
		actor->SetScale(0.5f, 0.5f, 0.5f);
		actor->AddComponent<CCPhysicalMove>();
		actor->AddComponent(
			MeshComponent::MeshID::kBasicSphere,
			m_pRenderer,
			m_phong_shader,
			L"./Data/Images/Sand_003_SD/Sand_003_COLOR.jpg"
			);

		actor->AddComponent<SphereColliderComponent>();
		m_pActorManager->AddActor(actor);
	}
	// Field
	m_field = Actor::Initialize(ActorID::kFloor);
	m_field->AddComponent(
		MeshComponent::MeshID::kBasicCube,
		m_pRenderer,
		m_phong_shader,
		L"./Data/Images/Test.png"
	);
	m_field->SetPosition(Vector3(0, -0.5f, 0));
	m_field->SetScale(20.0f, 0.5f, 20.0f);
	m_pActorManager->AddActor(m_field);


	m_audio_system = std::make_unique<AudioSystem>();
	m_audio_system->loadMusic(0, L"./Data/Musics/Bird_Ambience.wav");
	m_audio_system->playMusic(0, true);

	m_physics = std::make_unique<PhysicsManager>();
	m_physics->Init(m_pActorManager, true);



}

void SceneD::InitializeScene()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneD::Update(float elapsed_time)
{
	m_pCameraController->Update(elapsed_time);

	m_pLightController->Update(elapsed_time);

	m_pActorManager->Update(elapsed_time);

	m_physics->DetectCollision(m_pActorManager, elapsed_time);

}

//----------------------------------------------------------------------------------------------------------------------------

void SceneD::Render(std::unique_ptr<GraphicsEngine>& p_graphics,
	float elapsed_time)
{
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immContext = p_graphics->GetImmContextPtr();

	m_pCameraController->SetMatrix(immContext);

	m_pLightController->SetDataForGPU(immContext, m_pCameraController);

	m_pBlender->SetBlendState(immContext, Blender::BLEND_ALPHA);

	m_pRenderer->RenderQueue(immContext, elapsed_time);

	m_pBlender->SetBlendState(immContext, Blender::BLEND_ADD);

	mFont->Begin(immContext);

	mFont->RenderText(
		immContext,
		"player position :" +
		std::to_string(m_player->GetPosition().x) + ". " +
		std::to_string(m_player->GetPosition().y) + ". " +
		std::to_string(m_player->GetPosition().z)
		, XMFLOAT2(16, 24), XMFLOAT2(16, 16), XMFLOAT4(1, 1, 1, 1));
	mFont->RenderText(
		immContext,
		"player velocity :" +
		std::to_string(m_player->GetComponent<MoveComponent>()->GetVelocity().x) + ". " +
		std::to_string(m_player->GetComponent<MoveComponent>()->GetVelocity().y) + ". " +
		std::to_string(m_player->GetComponent<MoveComponent>()->GetVelocity().z)
		, XMFLOAT2(16, 40), XMFLOAT2(16, 16), XMFLOAT4(1, 1, 1, 1));

	mFont->RenderText(
		immContext,
		"collision counter :" +
		std::to_string(m_physics->GetCollisionCounter() / 2)
		, XMFLOAT2(16, 56), XMFLOAT2(16, 16), XMFLOAT4(1, 1, 1, 1));
	mFont->RenderText(
		immContext,
		"GamePad Connection : " +
		std::to_string(InputPtr->IsConnected(0)),
		XMFLOAT2(16, 72), XMFLOAT2(16, 16), XMFLOAT4(1, 1, 1, 1));
	

	mFont->End(immContext);
}

//----------------------------------------------------------------------------------------------------------------------------

SceneD::~SceneD()
{
}

//----------------------------------------------------------------------------------------------------------------------------
