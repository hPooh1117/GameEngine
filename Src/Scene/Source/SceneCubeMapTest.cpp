#include "SceneCubeMapTest.h"

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
#include "./Renderer/Skybox.h"
#include "./Renderer/Renderer.h"
#include "./Renderer/RenderTarget.h"
#include "./Renderer/NewMeshRenderer.h"

#include "./RenderPass/RenderPasses.h"
#include "./RenderPass/MakeCubeMapPasses.h"

//----------------------------------------------------------------------------------------------------------------------------

SceneF::SceneF(SceneManager* manager, Graphics::GraphicsDevice* p_device) :Scene(manager, p_device)
{
	mNextScene = SceneID::SCENE_A;


	ENGINE.GetLightPtr()->Init(0, 0);
	ENGINE.GetLightPtr()->SetLightColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));
	ENGINE.GetLightPtr()->SetShininess(30.0f);


	ENGINE.GetRenderer()->GetMeshRenderer()->SetSkybox(SkyboxTextureID::EGray0);

	ENGINE.GetMeshRenderer()->SetSkybox(SkyboxTextureID::EGray0);


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
	Actor* pFemale = new Actor();
	pFemale->SetScale(0.05f, 0.05f, 0.05f);
	pFemale->SetPosition(Vector3(0, 0.5f, 0));
	pFemale->AddComponent<MeshComponent>();
	pFemale->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::ESpotLightPhongForSkinning, L"./Data/Models/Female/Idle.fbx", FbxType::EMaya);
	pFemale->GetComponent<MeshComponent>()->RegisterMotion("Idle", L"./Data/Models/Female/Idle.fbx");
	pFemale->GetComponent<MeshComponent>()->RegisterMotion("Walking", L"./Data/Models/Female/Walking.fbx");
	pFemale->GetComponent<MeshComponent>()->RegisterMotion("Running", L"./Data/Models/Female/Running.fbx");
	pFemale->GetComponent<MeshComponent>()->RegisterAdditionalShader(ShaderID::EMakeCubeMapForSkinning, ShaderUsage::ECubeMap);
	ENGINE.GetActorManagerPtr()->AddActor(pFemale, count++);

	pFemale->GetComponent<MeshComponent>()->Play("Walking");

	Actor* pSphere = new Actor();
	pSphere->SetScale(5.0f, 5.0f, 5.0f);
	pSphere->SetPosition(Vector3(0, 5.0f, 4.0f));
	pSphere->AddComponent<MoveRoundTrip>();
	pSphere->AddComponent<MeshComponent>();
	pSphere->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::EUseCubeMap, nullptr, FbxType::EDefault);
	pSphere->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/PBR/cgbookcase/blue-tiles-01/Blue_tiles_01_2K_Base_Color.png", TextureConfig::EColorMap);
	ENGINE.GetActorManagerPtr()->AddActor(pSphere, count++);


	Actor* pField = new Actor();
	pField->SetPosition(Vector3(0, 0, 0));
	pField->SetScale(100.0f, 0.1f, 100.0f);
	pField->AddComponent<MeshComponent>();
	pField->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicCube, ShaderID::ESpotLightPhong, nullptr, FbxType::EDefault);
	pField->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/Test.png", TextureConfig::EColorMap);
	pField->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Models/OBJ/sea/Nsea.png", TextureConfig::ENormalMap);
	pField->GetComponent<MeshComponent>()->RegisterAdditionalShader(ShaderID::EMakeCubeMap, ShaderUsage::ECubeMap);

	ENGINE.GetActorManagerPtr()->AddActor(pField, count++);

	ENGINE.GetCameraPtr()->SetTarget(pFemale);
	ENGINE.GetCameraPtr()->SetPositionOfMoveableCamera(Vector3(0.0f, 10.0f, -60.0f));

}

void SceneF::InitializeScene()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SceneF::Update(float elapsed_time)
{
	Vector3 pos = ENGINE.GetCameraPtr()->GetCameraPosition();
	pos.y = ENGINE.GetActorManagerPtr()->GetActor(ActorID::EField)->GetPosition().y - pos.y;

	MakeCubeMapPass* pRenderPass = (MakeCubeMapPass*)ENGINE.GetRenderer()->GetRenderPass(RenderPassID::ECubeMapPass);
	Actor* sphere = ENGINE.GetActorManagerPtr()->GetActor(ActorID::ESphere);
	pRenderPass->SetOriginPoint(sphere->GetPosition(), sphere->GetScaleValue() * 0.5f);

}

void SceneF::PreCompute(Graphics::GraphicsDevice* p_graphics)
{

}

//----------------------------------------------------------------------------------------------------------------------------

void SceneF::Render(Graphics::GraphicsDevice* p_graphics, float elapsed_time)
{
}

void SceneF::RenderUI()
{
	auto* ui = ENGINE.GetRenderer()->GetUIRenderer();
	Actor* player = ENGINE.GetActorManagerPtr()->GetActor(ActorID::EFemale);
	ui->SetNextWindowSettings(Vector2(0, SCREEN_HEIGHT - 200), Vector2(300, 200));
	ui->BeginRenderingNewWindow("Motion");
	if (ImGui::Button("Idle")) player->GetComponent<MeshComponent>()->Play("Idle");
	if (ImGui::Button("Walking")) player->GetComponent<MeshComponent>()->Play("Walking");
	if (ImGui::Button("Running")) player->GetComponent<MeshComponent>()->Play("Running");

	ui->FinishRenderingWindow();

}

//----------------------------------------------------------------------------------------------------------------------------
