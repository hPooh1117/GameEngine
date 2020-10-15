#include "GameSystem.h"

#include <iostream>

#include "./Application/ThreadPool.h"

#include "./Renderer/GraphicsEngine.h"

#include "./Scene/SceneManager.h"

#include "./Utilities/Log.h"
#include "./Utilities/PerfTimer.h"

const unsigned int GameSystem::THREAD_SIZE = ThreadPool::MAX_NUM_THREADS - 2;
std::mutex         GameSystem::mLoadingMutex;

GameSystem::GameSystem()
    :m_pTimer(std::make_unique<PerfTimer>()),
    m_pThreadPool(std::make_unique<ThreadPool>(THREAD_SIZE)),
    m_bIsLoading(false)
{
}

void GameSystem::Initialize(D3D::DevicePtr& p_device)
{
    Log::Info("[ENGINE] Initializing ----------------------------------------------");

    m_pTimer->Start();



    m_pSceneManager = std::make_unique<SceneManager>(p_device);

    m_pSceneManager->InitializeLoadingScene();
    
    m_bIsLoading = true;
    auto AsyncLoad = [&]()
    {
        Log::Info("[ENGINE] Start Loading(Async)");

        PerfTimer timer;
        timer.Start();

        std::unique_lock<std::mutex> lck(mLoadingMutex);
        {
            m_pSceneManager->InitializeCurrentScene();
        }
        m_bIsLoading = false;


        timer.Stop();
        Log::Info("[ENGINE] Loaded(Async) in %.2f---------------", timer.GetDeltaTime());
        return true;
    };

    m_pThreadPool->Enqueue(AsyncLoad);

    m_pTimer->Stop();


    Log::Info("[ENGINE] Initialized ( %.2f s )-------------------------------------", m_pTimer->GetDeltaTime());

}

void GameSystem::Update(float elapsed_time)
{
    if (m_bIsLoading)
    {
        m_pSceneManager->ExecuteLoadingScene(elapsed_time);
    }
    if (!m_bIsLoading)
    {
        m_pSceneManager->ExecuteCurrentScene(elapsed_time);

    }
}

void GameSystem::Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time)
{
    if (m_bIsLoading)
    {
        m_pSceneManager->RenderLoadingScene(p_graphics, elapsed_time);

    }
    if (!m_bIsLoading)
    {
        m_pSceneManager->RenderCurrentScene(p_graphics, elapsed_time);

    }
}

void GameSystem::LoadScene()
{
}


GameSystem::~GameSystem()
{
}
