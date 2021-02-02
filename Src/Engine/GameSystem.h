#pragma once
#include <memory>
#include <vector>
#include <mutex>

#include "Singleton.h"

#include "./Renderer/D3D_Helper.h"
#include "./Renderer/GraphicsDevice.h"


class SceneManager;
class PerfTimer;
class ThreadPool;

class Actor;
class ActorManager;
class CameraController;
class LightController;
class ComputeExecuter;
class Renderer;

namespace Settings { struct Renderer; }

class GameSystem
{
public:
    static const unsigned int THREAD_SIZE;


public:
    static std::mutex                 mLoadingMutex;
    static unsigned int mCurrentWidth;
    static unsigned int mCurrentHeight;

private:                              
    std::unique_ptr<SceneManager>     mpSceneManager;
    std::unique_ptr<PerfTimer>        mpTimer;
    std::shared_ptr<ThreadPool>       mpThreadPool;

    bool mbIsLoadingRenderPasses;
    bool mbIsLoadingScene;

    std::unique_ptr<ActorManager>     mpActorManager;
    std::unique_ptr<CameraController> mpCamera;
    std::unique_ptr<LightController>  mpLight;
    std::unique_ptr<ComputeExecuter>  mpComputeExecuter; // コンピュートシェーダを利用した計算担当クラス


    bool mbIsCastingShadow;
    bool mbIsDeffered;
    bool mbIsSSAO;
    bool mbIsCubeMap;

    std::unique_ptr<Renderer> mpRenderer;

    u_int mCurrentPass;
    unsigned int mCurrentScreenNo;

    float mFrameTimer = .0f;

public:
    GameSystem();
    ~GameSystem();

    void Initialize(std::unique_ptr<Graphics::GraphicsDevice>& p_graphics);
    void Update(std::unique_ptr<Graphics::GraphicsDevice>& p_graphics, float elapsed_time);
    void Render(std::unique_ptr<Graphics::GraphicsDevice>& p_graphics, float elapsed_time);
    void Clear();

public:
    void LoadScene();
    void LoadNextScene(std::unique_ptr<Graphics::GraphicsDevice>& p_graphics);
    void LoadRenderPasses(std::unique_ptr<Graphics::GraphicsDevice>& p_graphics);
    void CheckShaderActivated();


    void SetRendererSettings(const Settings::Renderer& settings);
    void SetCurrentWindowSize(D3D::DevicePtr& p_device, unsigned int width, unsigned int height);

    std::unique_ptr<ActorManager>&      GetActorManagerPtr() { return mpActorManager; }
    std::unique_ptr<CameraController>&  GetCameraPtr()       { return mpCamera; }
    std::unique_ptr<LightController>&   GetLightPtr()        { return mpLight; }
    Renderer* GetRenderer() { return mpRenderer.get(); }
    std::unique_ptr<ComputeExecuter>& GetComputeExecuter() { return mpComputeExecuter; }
    std::shared_ptr<ThreadPool>& GetThreadPoolPtr() { return mpThreadPool; }
    SceneManager* GetSceneManager() { return mpSceneManager.get(); }
    bool IsCastingShadow() { return mbIsCastingShadow; }
    bool IsSSAOActivated() { return mbIsSSAO; }
    bool IsDefferedRendering() { return mbIsDeffered; }
};

#define  ENGINE Singleton<GameSystem>::Get()
 