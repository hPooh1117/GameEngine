#include "SceneC.h"
#include "./Application/Application.h"
#include "./Application/Input.h"

#include "./Component/MeshComponent.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/PointLight.h"
#include "./Engine/SpotLight.h"
#include "./Engine/DirectionalLight.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/LightController.h"
#include "./Engine/GameSystem.h"
#include "./Engine/Settings.h"

#include "./Component/MoveRotationComponent.h"

#include "./Renderer/Blender.h"
#include "./Renderer/ShadowMap.h"
#include "./Renderer/MultiRenderTarget.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Shader.h"


using namespace DirectX;

SceneC::SceneC(SceneManager* manager, Graphics::GraphicsDevice* p_device) :Scene(manager,p_device)
{
	mNextScene = SceneID::SCENE_D;

	int count = 0;
	// ----------------------------------------------------------------------------------------------
	// アクター・コンポーネント作成
	// ----------------------------------------------------------------------------------------------
	Actor* pPlayer = new Actor();
	pPlayer->AddComponent<MoveRotationComponent>();
	pPlayer->AddComponent<MeshComponent>();
	pPlayer->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_BasicSphere, ShaderID::ESpotLightBump, nullptr, FbxType::EDefault);
	pPlayer->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/earthmap.jpg", TextureConfig::EColorMap);
	pPlayer->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/earthnormal.jpg", TextureConfig::ENormalMap);
	pPlayer->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Images/earthbump.jpg", TextureConfig::EHeightMap);
	pPlayer->SetScale(5, 5, 5);
	pPlayer->SetPosition(Vector3(0, 4, 0));
	ENGINE.GetActorManagerPtr()->AddActor(pPlayer, count++);

	Actor* pCat = new Actor();
	pCat->AddComponent<MeshComponent>();
	pCat->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::ESpotLightPhong, L"./Data/Models/oswell/cat_mdl.fbx", FbxType::EMaya);
	pCat->SetPosition(Vector3(4, 1, 0));
	pCat->SetScale(0.5f, 0.5f, 0.5f);
	ENGINE.GetActorManagerPtr()->AddActor(pCat, count++);

	Actor* pOswell = new Actor();
	pOswell->AddComponent<MeshComponent>();
	pOswell->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::ESpotLightPhongForSkinning, L"./Data/Models/oswell/oswell_test1.fbx", FbxType::EMaya);
	pOswell->GetComponent<MeshComponent>()->RegisterMotion("Move", L"./Data/Models/oswell/oswell_movemove.fbx");
	pOswell->SetPosition(Vector3(-4, 3, 0));
	pOswell->SetScale(0.05f, 0.05f, 0.05f);
	ENGINE.GetActorManagerPtr()->AddActor(pOswell, count++);

	Actor* pShiba = new Actor();
	pShiba->AddComponent<MeshComponent>();
	pShiba->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_SkinnedMesh, ShaderID::ESpotLightBump, L"./Data/Models/0810/shiba_mdl.fbx", FbxType::EMaya);
	pShiba->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Models/0810/shina_N.png", TextureConfig::ENormalMap);
	pShiba->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Models/0810/shiba_H.png", TextureConfig::EHeightMap);
	pShiba->SetPosition(Vector3(0, 0.5f, -4));
	ENGINE.GetActorManagerPtr()->AddActor(pShiba, count++);

	Actor* pField = new Actor();
	pField->AddComponent<MeshComponent>();
	pField->GetComponent<MeshComponent>()->RegisterMesh(MeshTypeID::E_StaticMesh, ShaderID::ESpotLightOcean, L"./Data/Models/OBJ/sea/sea.obj", FbxType::EDefault);
	pField->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Models/OBJ/sea/Nsea.png", TextureConfig::ENormalMap);
	pField->GetComponent<MeshComponent>()->RegisterTexture(L"./Data/Models/OBJ/sea/Hsea.png", TextureConfig::EHeightMap);
	pField->SetPosition(Vector3(0, 0, 0));
	pField->SetScale(200, 200, 200);
	ENGINE.GetActorManagerPtr()->AddActor(pField, count++);


	ENGINE.GetLightPtr()->Init(0, 4);
	ENGINE.GetLightPtr()->SetLightColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
	ENGINE.GetLightPtr()->SetSpotData(0, Vector3(-5, 5, -5), Vector4(0.9f, 0.2f, 0.2f, 1.0f), Vector3(1.0f, -0.2f, 0.0f), 50.0f, 0.99f, 0.8f);
	ENGINE.GetLightPtr()->SetSpotData(1, Vector3(-5, 5, 5), Vector4(0.2f, 0.2f, 0.9f, 1.0f), Vector3(-1.0f, -0.2f, 0.0f), 50.0f, 0.99f, 0.8f);
	ENGINE.GetLightPtr()->SetSpotData(2, Vector3(5, 5, -5), Vector4(0.2f, 0.9f, 0.2f, 1.0f), Vector3(0.0f, -0.2f, 1.0f), 50.0f, 0.99f, 0.8f);
	ENGINE.GetLightPtr()->SetSpotData(3, Vector3(5, 5, 5), Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector3(0.0f, -0.2f, -1.0f), 50.0f, 0.99f, 0.8f);
	ENGINE.GetLightPtr()->SetShininess(30.0f);


	ENGINE.GetCameraPtr()->SetTarget(pPlayer);
	ENGINE.GetCameraPtr()->SetPositionOfMoveableCamera(Vector3(0.0f, 10.0f, -60.0f));

	Settings::Renderer renderSettings = {
	false,   // shadow
	false,  // ssao
	false,   // deffered
	false   // cubemap
	};
	ENGINE.SetRendererSettings(renderSettings);


}

void SceneC::InitializeScene()
{
}

void SceneC::Update(float elapsed_time)
{
}

void SceneC::PreCompute(Graphics::GraphicsDevice* p_graphics)
{
}

void SceneC::Render(Graphics::GraphicsDevice* p_graphics,
	float elapsed_time)
{
}

void SceneC::RenderUI()
{
}

SceneC::~SceneC()
{
}
