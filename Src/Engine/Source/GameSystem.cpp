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

#include "./Renderer/NewMeshRenderer.h"
#include "./Renderer/Shader.h"
#include "./Renderer/ShadowMap.h"
#include "./Renderer/Skybox.h"
#include "./Renderer/Sprite.h"
#include "./Renderer/Texture.h"
#include "./Renderer/TextureHolder.h"
#include "./Renderer/Renderer.h"


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
	mpComputeExecuter(std::make_unique<ComputeExecuter>()),
	mbIsLoadingRenderPasses(false),
	mbIsLoadingScene(false),
	mbIsCastingShadow(false),
	mbIsDeffered(false),
	mbIsSSAO(false),
	mCurrentScreenNo(0),
	mpRenderer(std::make_unique<Renderer>())
{

}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::Initialize(std::unique_ptr<Graphics::GraphicsDevice>& p_device)
{
	D3D::DevicePtr& pDevice = p_device->GetDevicePtr();

	Log::Info("[ENGINE] Initializing ----------------------------------------------");

	mpTimer->Start();

	mpLight = std::make_unique<LightController>(pDevice);

	mpSceneManager = std::make_unique<SceneManager>(p_device.get());

	mpSceneManager->InitializeLoadingScene();

	mpRenderer->SetUp(mpSceneManager->GetNextSceneSettings());
	mpRenderer->Initialize(p_device.get());

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
			mpRenderer->GetMeshRenderer()->RegistMeshDataFromActors(p_device.get(), mpActorManager.get());
		}
		if (!mpRenderer->GetTextureHolderPtr()->Initialize(pDevice))
		{
			Log::Error("Couldn't Initialized TextureHolder.");
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpRenderer->GetTextureHolderPtr()->RegisterTextureFromActors(pDevice, mpActorManager.get());
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpComputeExecuter->Initialize(pDevice);
		}

		mbIsLoadingScene = false;
		timer.Stop();
		Log::Info("[ENGINE] Loaded Scene(Async) in %.2f(s)---------------", timer.GetDeltaTime());

		return true;
	};

	LoadRenderPasses(p_device);
	mpThreadPool->Enqueue(AsyncLoadScene);

	mpTimer->Stop();


	Log::Info("[ENGINE] Initialized in %.2f(s) -------------------------------------", mpTimer->GetDeltaTime());

}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::Update(std::unique_ptr<Graphics::GraphicsDevice>& p_graphics, float elapsed_time)
{
	if (mpSceneManager->IsLoading())
	{
		if (!mbIsLoadingScene)		  LoadNextScene(p_graphics);
		if (!mpRenderer->IsLoading()) LoadRenderPasses(p_graphics);
	}
	if (mpRenderer->IsLoading() || mbIsLoadingScene)
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
	mpActorManager->Update(elapsed_time);
	mpSceneManager->ExecuteCurrentScene(elapsed_time);

	mpTimer->Stop();
	mFrameTimer = mpTimer->GetDeltaTime();
	mpRenderer->Update(p_graphics.get());

	InputPtr.SetMouseWheelState(MouseState::ENone);

}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::Render(std::unique_ptr<Graphics::GraphicsDevice>& p_graphics, float elapsed_time)
{

	if (mpRenderer->IsLoading() || mbIsLoadingScene)
	{

		mpSceneManager->RenderLoadingScene(p_graphics.get(), elapsed_time);
		return;
	}


	D3D::DeviceContextPtr pImmContext = p_graphics->GetImmContextPtr();


	mpCamera->SetMatrix(pImmContext);

	mpLight->SetDataForGPU(pImmContext, mpCamera.get());

	mpRenderer->Render(p_graphics.get(), elapsed_time);
}


//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::LoadScene()
{
}

void GameSystem::LoadNextScene(std::unique_ptr<Graphics::GraphicsDevice>& p_graphics)
{
	D3D::DevicePtr& pDevice = p_graphics->GetDevicePtr();

	mbIsLoadingScene = true;

	mpRenderer->SetUp(mpSceneManager->GetNextSceneSettings());


	auto asyncLoadNextScene = [&]()
	{
		Log::Info("[ENGINE] Start Loading(Async)");

		PerfTimer timer;
		timer.Start();
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpRenderer->Clear();
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpActorManager->ClearAll();
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpSceneManager->LoadNextScene();
			mpRenderer->SetUp(mpSceneManager->GetNextSceneSettings());

		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpRenderer->GetMeshRenderer()->RegistMeshDataFromActors(p_graphics.get(), mpActorManager.get());
		}
		{
			std::unique_lock<std::mutex> lck(mLoadingMutex);
			mpRenderer->GetTextureHolderPtr()->RegisterTextureFromActors(pDevice, mpActorManager.get());
		}

		mbIsLoadingScene = false;

		timer.Stop();
		Log::Info("[ENGINE] Loaded(Async) in %.2f(s)---------------", timer.GetDeltaTime());
	};
	mpThreadPool->Enqueue(asyncLoadNextScene);
}



void GameSystem::LoadRenderPasses(std::unique_ptr<Graphics::GraphicsDevice>& p_graphics)
{
	if (mpRenderer->IsInitialized()) return;
	mpRenderer->InitializeAsync(p_graphics.get());
}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::CheckShaderActivated()
{
	mpRenderer->CheckShaderActivated();
}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::SetRendererSettings(const Settings::Renderer& settings)
{
	mbIsCastingShadow = settings.bCastShadow;
	mbIsDeffered = settings.bIsDeferred;
	mbIsSSAO = settings.bEnableAO;
	mbIsCubeMap = settings.bCubeMap;
}

void GameSystem::SetCurrentWindowSize(D3D::DevicePtr& p_device, unsigned int width, unsigned int height)
{
	mCurrentWidth = width;
	mCurrentHeight = height;
}

//--------------------------------------------------------------------------------------------------------------------------------

GameSystem::~GameSystem()
{
}

//--------------------------------------------------------------------------------------------------------------------------------
