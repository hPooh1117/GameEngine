#include "GameSystem.h"

#include <iostream>

#include "./Application/ThreadPool.h"
#include "./Application/Input.h"

#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/LightController.h"
#include "./Engine/UIRenderer.h"

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
#include "./RenderPass/DefferedPasses.h"
#include "./RenderPass/SSAOPasses.h"

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
    mpDefferedPass(std::make_unique<DefferedPass>()),
    mpSSAOPass(std::make_unique<SSAOPass>()),
    mpMeshRenderer(std::make_unique<NewMeshRenderer>()),
    mpUIRenderer(nullptr),
    mpTextureHolder(std::make_unique<TextureHolder>()),
    //mpMeshRenderer(std::make_unique<NewMeshRenderer>()),
    m_bIsLoading(false),
    mbIsCastingShadow(false),
    mbIsDeffered(false),
    mbIsSSAO(false),
    mCurrentScreenNo(0)
{
    
}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::Initialize(D3D::DevicePtr& p_device)
{
    Log::Info("[ENGINE] Initializing ----------------------------------------------");

    mpTimer->Start();

    mpLight = std::make_unique<LightController>(p_device);

    mpUIRenderer = std::make_unique<UIRenderer>(p_device);

    mpMeshRenderer->Initialize(p_device);

    m_pSceneManager = std::make_unique<SceneManager>(p_device);

    m_pSceneManager->InitializeLoadingScene();
    
    m_bIsLoading = true;
    auto AsyncLoad = [&]()
    {
        Log::Info("[ENGINE] Start Loading(Async)");

        PerfTimer timer;
        timer.Start();
        {
            std::unique_lock<std::mutex> lck(mLoadingMutex);
            mpShadowPass->Initialize(p_device);
        }
        {
            std::unique_lock<std::mutex> lck(mLoadingMutex);
            mpForwardPass->Initialize(p_device);
        }
        {
            std::unique_lock<std::mutex> lck(mLoadingMutex);
            mpPostProcessPass->Initialize(p_device);
        }
        {
            std::unique_lock<std::mutex> lck(mLoadingMutex);
            mpDefferedPass->Initialize(p_device);
        }
        {
            std::unique_lock<std::mutex> lck(mLoadingMutex);
            mpSSAOPass->Initialize(p_device);
        }
        {
            std::unique_lock<std::mutex> lck(mLoadingMutex);
            m_pSceneManager->InitializeCurrentScene();
        }
        {
            std::unique_lock<std::mutex> lck(mLoadingMutex);
            mpMeshRenderer->RegistMeshDataFromActors(p_device, mpActorManager);
        }
        {
            std::unique_lock<std::mutex> lck(mLoadingMutex);
            if (!mpTextureHolder->Initialize(p_device))
            {
                Log::Error("Couldn't Initialized TextureHolder.");
            }
            mpTextureHolder->RegistTextureFromActors(p_device, mpActorManager);
        }
        m_bIsLoading = false;


        timer.Stop();
        Log::Info("[ENGINE] Loaded(Async) in %.2f(s)---------------", timer.GetDeltaTime());
        return true;
    };

    mpThreadPool->Enqueue(AsyncLoad);

    mpTimer->Stop();


    Log::Info("[ENGINE] Initialized in %.2f(s) -------------------------------------", mpTimer->GetDeltaTime());

}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::Update(D3D::DevicePtr& p_device, float elapsed_time)
{
    if (m_pSceneManager->GetLoadingScene() == true && !m_bIsLoading)
    {
        m_bIsLoading = true;
        auto asyncLoadNextScene = [&]()
        {
            Log::Info("[ENGINE] Start Loading(Async)");

            PerfTimer timer;
            timer.Start();
            {
                std::unique_lock<std::mutex> lck(mLoadingMutex);
                mpActorManager->ClearAll();
            }
            {
                std::unique_lock<std::mutex> lck(mLoadingMutex);
                mpUIRenderer->ClearUIClients();
            }
            {
                std::unique_lock<std::mutex> lck(mLoadingMutex);
                m_pSceneManager->LoadNextScene();
            }
            {
                std::unique_lock<std::mutex> lck(mLoadingMutex);
                mpMeshRenderer->ClearAll();
                mpMeshRenderer->RegistMeshDataFromActors(p_device, mpActorManager);
            }
            {
                std::unique_lock<std::mutex> lck(mLoadingMutex);
                mpTextureHolder->ClearTextureTable();
                mpTextureHolder->RegistTextureFromActors(p_device, mpActorManager);
            }

            m_bIsLoading = false;

            timer.Stop();
            Log::Info("[ENGINE] Loaded(Async) in %.2f(s)---------------", timer.GetDeltaTime());
        };
        mpThreadPool->Enqueue(asyncLoadNextScene);
    }


    if (m_bIsLoading)
    {
        m_pSceneManager->ExecuteLoadingScene(elapsed_time);
        return;
    }

    mpCamera->Update(elapsed_time);
    mpLight->Update(elapsed_time);


    m_pSceneManager->ExecuteCurrentScene(elapsed_time);
    mpActorManager->Update(elapsed_time);

    InputPtr.SetMouseWheelState(MouseState::ENone);
}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{

    if (m_bIsLoading)
    {
        m_pSceneManager->RenderLoadingScene(p_graphics, elapsed_time);
        return;
    }


    D3D::DeviceContextPtr pImmContext = p_graphics->GetImmContextPtr();

    mpUIRenderer->UpdateFrame();

    mpCamera->SetMatrix(pImmContext);

    mpLight->SetDataForGPU(pImmContext, mpCamera.get());

    mpUIRenderer->SetNextWindowSettings(Vector2(SCREEN_WIDTH - 300, 0), Vector2(300, 600));
    mpUIRenderer->BeginRenderingNewWindow("RenderTatgets");

    if (mbIsCastingShadow) mpShadowPass->RenderShadow(p_graphics, elapsed_time);

    if (!mbIsDeffered)     mpForwardPass->RenderForwardLighting(p_graphics, elapsed_time);

    if (mbIsDeffered)      mpDefferedPass->RenderDefferedLighting(p_graphics, elapsed_time);

    if (mbIsSSAO)          mpSSAOPass->ExecuteSSAO(p_graphics, elapsed_time);

    RenderUIByRenderPasses();

    mpPostProcessPass->RenderPostProcess(p_graphics, elapsed_time);

    m_pSceneManager->RenderCurrentScene(p_graphics, elapsed_time);

    mpUIRenderer->FinishRenderingWindow();

    m_pSceneManager->RenderUI();

    RenderUI(p_graphics);
}

void GameSystem::RenderUIByRenderPasses()
{
    if (mbIsCastingShadow) mpShadowPass->RenderUI();
    if (!mbIsDeffered)     mpForwardPass->RenderUI();
    if (mbIsDeffered)      mpDefferedPass->RenderUI();
    if (mbIsSSAO)          mpSSAOPass->RenderUI();
}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::RenderUI(std::unique_ptr<GraphicsEngine>& p_graphics)
{
    mpUIRenderer->SetNextWindowSettings(Vector2(SCREEN_WIDTH - 300, 600), Vector2(300, SCREEN_HEIGHT - 600));
    mpUIRenderer->BeginRenderingNewWindow("Debugging");
    
    if (mbIsSSAO) mpSSAOPass->RenderUIForRuntime();
    mpCamera->RenderUI();
    mpLight->RenderUI();
    mpMeshRenderer->RenderUI();

    mpUIRenderer->FinishRenderingWindow();


    mpUIRenderer->Execute();

    p_graphics->mBlender.SetBlendState(p_graphics->GetImmContextPtr(), Blender::BLEND_ADD);

    mpUIRenderer->RenderSpriteFontQueue(p_graphics->GetImmContextPtr());

    p_graphics->mBlender.SetBlendState(p_graphics->GetImmContextPtr(), Blender::BLEND_ALPHA);

}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::LoadScene()
{
}

//--------------------------------------------------------------------------------------------------------------------------------

void GameSystem::SetCurrentWindowSize(D3D::DevicePtr& p_device, unsigned int width, unsigned int height)
{
    mCurrentWidth = width;
    mCurrentHeight = height;

    mpShadowPass->GetRenderTargetPtr()->Resize(p_device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
    mpForwardPass->GetRenderTargetPtr()->Resize(p_device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
}

RenderPass* GameSystem::GetRenderPass(unsigned int render_pass_id)
{
    switch (render_pass_id)
    {
    case RenderPassID::EForwardPass:
        return mpForwardPass.get();
    case RenderPassID::EShadowPass:
        return mpShadowPass.get();
    case RenderPassID::EDefferedPass:
        return mpDefferedPass.get();
    case RenderPassID::EPostEffectPass:
        return mpPostProcessPass.get();
    case RenderPassID::ESSAOPass:
        return mpSSAOPass.get();
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
