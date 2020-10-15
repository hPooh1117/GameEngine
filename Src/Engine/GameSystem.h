#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <mutex>

#include "./Renderer/D3D_Helper.h"

class SceneManager;
class GraphicsEngine;
class PerfTimer;
class ThreadPool;

class GameSystem
{
public:
    static const unsigned int THREAD_SIZE;

public:
    static std::mutex             mLoadingMutex;
private:
    std::unique_ptr<SceneManager> m_pSceneManager;
    std::unique_ptr<PerfTimer>    m_pTimer;

    std::shared_ptr<ThreadPool>   m_pThreadPool;

    bool m_bIsLoading;
public:
    GameSystem();
    ~GameSystem();

    void Initialize(D3D::DevicePtr& p_device);
    void Update(float elapsed_time);
    void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);

    void LoadScene();
};