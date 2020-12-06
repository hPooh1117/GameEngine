#include "GameSystem.h"

#include <iostream>

#include "./Application/ThreadPool.h"
#include "./Application/Input.h"

#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/LightController.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/ComputeExecuter.h"
#include "./Engine/Settings.h"
#include "./Engine/OrthoView.h"

#include "./Renderer/RenderTarget.h"
#include "./Renderer/DepthStencilView.h"

#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/Shader.h"
#include "./Renderer/ShadowMap.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Texture.h"
#include "./Renderer/TextureHolder.h"


#include "./RenderPass/ForwardPasses.h"
#include "./RenderPass/ShadowPasses.h"
#include "./RenderPass/PostProcessPasses.h"
#include "./RenderPass/DeferredPasses.h"
#include "./RenderPass/SSAOPasses.h"
#include "./RenderPass/MakeCubeMapPasses.h"

#include "./Scene/SceneManager.h"

#include "./Utilities/Log.h"
#include "./Utilities/PerfTimer.h"


//--------------------------------------------------------------------------------------------------------------------------------

const unsigned int GameSystem::THREAD_SIZE = ThreadPool::MAX_NUM_THREADS - 2;
std::mutex         GameSystem::mLoadingMutex;
unsigned int       GameSystem::mCurrentWidth = SCREEN_WIDTH;
unsigned int       GameSystem::mCurrentHeight = SCREEN_HEIGHT;

//--------------------------------------------------------------------------------------------------------------------------------

GameSystem::GameSystem()
	:mpTimer(std::make_unique<PerfTimer>()),
	mpThreadPool(std::make_unique<ThreadPool>(THREAD_SIZE)),
	mpActorManager(std::make_unique<ActorManager>()),
	mpCamera(std::make_unique<CameraController>()),
	mpLight(nullptr),
	mpForwardPass(std::make_unique<ForwardPass>()),
	mpShadowPass(std::make_unique<ShadowPass>()),
	mpPostProcessPass(std::make_unique<PostProcessPass>()),
	mpDefferedPass(std::make_unique<DeferredPass>()),
	mpSSAOPass(std::make_unique<SSAOPass>()),
	mpCubeMapPass(std::make_unique<MakeCubeMapPass>()),
	mpMeshRenderer(std::make_unique<NewMeshRenderer>()),
	mpUIRenderer(nullptr),
	mpComputeExecuter(std::make_unique<ComputeExecuter>()),
	mpTextureHolder(std::make_unique<TextureHolder>()),
	//mpMeshRenderer(std::make_unique<NewMeshRenderer>()),
	mbIsLoadingRenderPasses(false),
	mbIsLoadingScene(false),
	mbIsCastingShadow(false),
	mbIsDeffered(false),
	mbIsSSAO(false),
	mCurrentScreenNo(0)
{

}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::Initialize(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	D3D::DevicePtr& p_device = p_graphics->GetDevicePtr();

	Log::Info("[ENGINE] Initializing ----------------------------------------------");

	mpTimer->Start();

	mpLight = std::make_unique<LightController>(p_device);

	mpUIRenderer = std::make_unique<UIRenderer>(p_device);

	mpMeshRenderer->Initialize(p_device);

	mpSceneManager = std::make_unique<SceneManager>(p_device);

	mpSceneManager->InitializeLoadingScene();

	Settings::Renderer currentSettings = mpSceneManager->GetNextSceneSettings();
	mbIsCastingShadow = currentSettings.bCastShadow;
	mbIsDeffered = currentSettings.bIsDeffered;
	mbIsSSAO = currentSettings.bEnableAO;
	mbIsCubeMap = currentSettings.bCubeMap;

	mbIsLoadingScene = true;

	auto AsyncLoadScene = [&]()
	{
		Log::Info("[ENGINE] Start Loading Scene(Async)");

		PerfTimer timer;
		timer.Start();

		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpSceneManager->InitializeCurrentScene();
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpMeshRenderer->RegistMeshDataFromActors(p_device, mpActorManager.get());
		}
		if (!mpTextureHolder->Initialize(p_device))
		{
			Log::Error("Couldn't Initialized TextureHolder.");
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpTextureHolder->RegisterTextureFromActors(p_device, mpActorManager.get());
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpComputeExecuter->Initialize(p_device);
		}

		mbIsLoadingScene = false;
		timer.Stop();
		Log::Info("[ENGINE] Loaded Scene(Async) in %.2f(s)---------------", timer.GetDeltaTime());

		return true;
	};

	LoadRenderPasses(p_graphics);
	mpThreadPool->Enqueue(AsyncLoadScene);

	mpTimer->Stop();


	Log::Info("[ENGINE] Initialized in %.2f(s) -------------------------------------", mpTimer->GetDeltaTime());

}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::Update(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	if (mpSceneManager->IsLoading())
	{
		if (!mbIsLoadingScene)		  LoadNextScene(p_graphics);
		if (!mbIsLoadingRenderPasses) LoadRenderPasses(p_graphics);
	}
	if (mbIsLoadingRenderPasses || mbIsLoadingScene)
	{
		mpSceneManager->ExecuteLoadingScene(elapsed_time);
		return;
	}


	mpCamera->Update(elapsed_time);
	mpLight->Update(elapsed_time);
#ifdef _DEBUG
	if (InputPtr.OnKeyTrigger("LeftCtrl")) CheckShaderActivated();
#endif // _DEBUG

	mpTimer->Reset();
	mpTimer->Start();
	mpSceneManager->ExecuteCurrentScene(elapsed_time);
	mpActorManager->Update(elapsed_time);
	mpTimer->Stop();
	mFrameTimer = mpTimer->GetDeltaTime();

	InputPtr.SetMouseWheelState(MouseState::ENone);

}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{

	if (mbIsLoadingRenderPasses || mbIsLoadingScene)
	{

		mpSceneManager->RenderLoadingScene(p_graphics, elapsed_time);
		return;
	}


	D3D::DeviceContextPtr pImmContext = p_graphics->GetImmContextPtr();

	mpUIRenderer->UpdateFrame();

	mpCamera->SetMatrix(pImmContext);

	mpLight->SetDataForGPU(pImmContext, mpCamera.get());


	if (mbIsCastingShadow) mpShadowPass->RenderShadow(p_graphics, elapsed_time);
	if (mbIsCubeMap)       mpCubeMapPass->MakeCubeMap(p_graphics, elapsed_time);
	if (!mbIsDeffered)     mpForwardPass->RenderForwardLighting(p_graphics, elapsed_time);
	if (mbIsDeffered)      mpDefferedPass->RenderDefferedLighting(p_graphics, elapsed_time);

	if (mbIsSSAO)          mpSSAOPass->ExecuteSSAO(p_graphics, elapsed_time);

	mpPostProcessPass->RenderPostProcess(p_graphics, elapsed_time);

	//mpFinalPass->RenderResult(p_graphics, elapsed_time);
	mpSceneManager->RenderCurrentScene(p_graphics, elapsed_time);

	RenderUI(p_graphics, elapsed_time);
}

void GameSystem::RenderUIForSettings(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	mpUIRenderer->SetNextWindowSettings(Vector2(SCREEN_WIDTH - 350, 23), Vector2(350, 337));
	mpUIRenderer->SetNextUIConfig(true);
	mpUIRenderer->BeginRenderingNewWindow("RenderTatgets");

	RenderUIByRenderPasses(p_graphics);
	mpUIRenderer->FinishRenderingWindow();



	mpUIRenderer->SetNextWindowSettings(Vector2(SCREEN_WIDTH - 350, 360), Vector2(350, SCREEN_HEIGHT - 360));
	mpUIRenderer->BeginRenderingNewWindow("Settings");

	if (mbIsDeffered) ImGui::Checkbox("Enable SSAO", &mbIsSSAO);
	if (mbIsDeffered) mpSSAOPass->RenderUIForSettings();
	mpCamera->RenderUI();
	mpLight->RenderUI();
	mpMeshRenderer->RenderUI();
	mpPostProcessPass->RenderUIForSettings();

	ImGui::Text("Elapsed Time on Update : %.5f ms", mFrameTimer);


	mpUIRenderer->FinishRenderingWindow();

}

void GameSystem::RenderUIByRenderPasses(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	bool bOpen = true;

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Tab"))
		{
			if (ImGui::MenuItem("Close All Tabs"))
			{
				bOpen = false;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (mbIsCastingShadow) mpShadowPass->RenderUI(bOpen);
	if (mbIsCubeMap)       mpCubeMapPass->RenderUI(bOpen);
	if (!mbIsDeffered)     mpForwardPass->RenderUI(bOpen);
	if (mbIsDeffered)      mpDefferedPass->RenderUI(bOpen);
	if (mbIsSSAO)          mpSSAOPass->RenderUI(bOpen);
	mpPostProcessPass->RenderUI();
}

void GameSystem::RenderUIForMainMenuBar(std::unique_ptr<GraphicsEngine>& p_graphics)
{
#ifdef _DEBUG
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (mbIsCastingShadow) mpShadowPass->ShowShaderList(p_graphics, "ShadowPass");
			if (mbIsCubeMap)       mpCubeMapPass->ShowShaderList(p_graphics, "CubemapPass");
			if (!mbIsDeffered)     mpForwardPass->ShowShaderList(p_graphics, "ForwardPass");
			if (mbIsDeffered)      mpDefferedPass->ShowShaderList(p_graphics, "DefferedPass");
			if (mbIsSSAO)          mpSSAOPass->ShowShaderList(p_graphics, "SSAOPass");
			mpPostProcessPass->ShowShaderList(p_graphics, "PostProcessPass");

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Enable UI"))
			{
				mpUIRenderer->SetUIEnable(true);
			}
			if (ImGui::MenuItem("Disable UI"))
			{
				mpUIRenderer->SetUIEnable(false);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	mpShadowPass->ReloadShaderFile(p_graphics);
	mpCubeMapPass->ReloadShaderFile(p_graphics);
	mpForwardPass->ReloadShaderFile(p_graphics);
	mpDefferedPass->ReloadShaderFile(p_graphics);
	mpSSAOPass->ReloadShaderFile(p_graphics);
	mpPostProcessPass->ReloadShaderFile(p_graphics);
#endif

}

void GameSystem::RenderUIForScene(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	ENGINE.GetUIRenderer()->SetNextWindowSettings(Vector2(0, 23), Vector2(150, 400));
	mpUIRenderer->SetNextUIConfig(true);
	ENGINE.GetUIRenderer()->BeginRenderingNewWindow("Scene", false);

	mpSceneManager->RenderUI();


	ENGINE.GetUIRenderer()->FinishRenderingWindow();

	mpSceneManager->RenderUIForCurrentScene();
}


//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::RenderUI(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
	if (mpUIRenderer->GetUIEnable())
	{
		RenderUIForSettings(p_graphics, elapsed_time);

		if (mbIsDeffered) mpDefferedPass->RenderUIForAnotherScreen();
		if (mbIsSSAO)     mpSSAOPass->RenderUIForAnotherScreen();

		RenderUIForScene(p_graphics, elapsed_time);

	}

	RenderUIForMainMenuBar(p_graphics);


	mpUIRenderer->Execute();

	p_graphics->mBlender.SetBlendState(p_graphics->GetImmContextPtr(), Blender::BLEND_ADD);

	mpUIRenderer->RenderSpriteFontQueue(p_graphics->GetImmContextPtr());

	p_graphics->mBlender.SetBlendState(p_graphics->GetImmContextPtr(), Blender::BLEND_ALPHA);

}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::LoadScene()
{
}

void GameSystem::LoadNextScene(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	D3D::DevicePtr& p_device = p_graphics->GetDevicePtr();

	mbIsLoadingScene = true;

	Settings::Renderer currentSettings = mpSceneManager->GetNextSceneSettings();
	mbIsCastingShadow = currentSettings.bCastShadow;
	mbIsDeffered = currentSettings.bIsDeffered;
	mbIsSSAO = currentSettings.bEnableAO;
	mbIsCubeMap = currentSettings.bCubeMap;


	auto asyncLoadNextScene = [&]()
	{
		Log::Info("[ENGINE] Start Loading(Async)");

		PerfTimer timer;
		timer.Start();
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpTextureHolder->ClearTextureTable();
			mpMeshRenderer->ClearAll();
			mpActorManager->ClearAll();
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpUIRenderer->ClearUIClients();
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpSceneManager->LoadNextScene();
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpMeshRenderer->RegistMeshDataFromActors(p_device, mpActorManager.get());
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpTextureHolder->RegisterTextureFromActors(p_device, mpActorManager.get());
		}

		mbIsLoadingScene = false;

		timer.Stop();
		Log::Info("[ENGINE] Loaded(Async) in %.2f(s)---------------", timer.GetDeltaTime());
	};
	mpThreadPool->Enqueue(asyncLoadNextScene);
}



void GameSystem::LoadRenderPasses(std::unique_ptr<GraphicsEngine>& p_graphics)
{
	D3D::DevicePtr& pDevice = p_graphics->GetDevicePtr();

	mbIsLoadingRenderPasses = true;

	auto AsyncLoadRenderPasses = [&]()
	{
		Log::Info("[ENGINE] Start Loading RenderPasses(Async)");

		PerfTimer timer;
		timer.Start();
		RenderPass::Clear();
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpPostProcessPass->Initialize(pDevice);
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			if (mbIsCastingShadow) mpShadowPass->Initialize(pDevice);
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			if (!mbIsDeffered) mpForwardPass->Initialize(pDevice);
			else               mpDefferedPass->Initialize(pDevice);
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			if (mbIsDeffered) mpSSAOPass->Initialize(pDevice);
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			if (mbIsCubeMap) mpCubeMapPass->Initialize(pDevice);
		}
		//{
		//    std::unique_lock<std::mutex> lck(mLoadingMutex);
		//    mpFinalPass->Create(p_device);
		//}

		mbIsLoadingRenderPasses = false;

		timer.Stop();
		Log::Info("[ENGINE] Loaded RenderPasses(Async) in %.2f(s)---------------", timer.GetDeltaTime());
		return true;
	};

	mpThreadPool->Enqueue(AsyncLoadRenderPasses);

}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::CheckShaderActivated()
{
	mpForwardPass->CheckActivatedShaders();
	mpDefferedPass->CheckActivatedShaders();
	mpShadowPass->CheckActivatedShaders();
	mpSSAOPass->CheckActivatedShaders();
	mpPostProcessPass->CheckActivatedShaders();
}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::SetRendererSettings(const Settings::Renderer& settings)
{
	mbIsCastingShadow = settings.bCastShadow;
	mbIsDeffered = settings.bIsDeffered;
	mbIsSSAO = settings.bEnableAO;
	mbIsCubeMap = settings.bCubeMap;
}

void GameSystem::SetCurrentWindowSize(D3D::DevicePtr& p_device, unsigned int width, unsigned int height)
{
	mCurrentWidth = width;
	mCurrentHeight = height;

	//mpShadowPass->GetRenderTargetPtr(RenderTarget::EShadow)->Resize(p_device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	//mpForwardPass->GetRenderTargetPtr(RenderTarget::EForward)->Resize(p_device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
}

RenderPass* GameSystem::GetRenderPass(unsigned int render_pass_id)
{
	switch (render_pass_id)
	{
	case RenderPassID::EForwardPass:
		return mpForwardPass.get();
	case RenderPassID::EShadowPass:
		return mpShadowPass.get();
	case RenderPassID::EDeferredPass:
		return mpDefferedPass.get();
	case RenderPassID::EPostEffectPass:
		return mpPostProcessPass.get();
	case RenderPassID::ESSAOPass:
		return mpSSAOPass.get();
	case RenderPassID::ECubeMapPass:
		return mpCubeMapPass.get();
	default:
		return mpForwardPass.get();
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

GameSystem::~GameSystem()
{
}

//--------------------------------------------------------------------------------------------------------------------------------
