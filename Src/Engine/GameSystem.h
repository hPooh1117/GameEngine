#pragma once
#include <memory>
#include <vector>
#include <mutex>

#include "Singleton.h"

#include "./Renderer/D3D_Helper.h"


class SceneManager;
class GraphicsEngine;
class PerfTimer;
class ThreadPool;

class Actor;
class ActorManager;
class CameraController;
class NewMeshRenderer;
class LightController;
class Skybox;
class TextureHolder;
class UIRenderer;
class ComputeExecuter;


class RenderPass;
class ForwardPass;
class ShadowPass;
class PostProcessPass;
class DeferredPass;
class SSAOPass;
class MakeCubeMapPass;

namespace Settings { struct Renderer; }

class GameSystem
{
public:
    static const unsigned int THREAD_SIZE;

    enum class Process
    {
        EUpdate,
        EShadowPass,
        EMainRenderPass,
        ESSAOPass,
        EPostProcessPass,
        ETotlaRender,
        EUIPass,

        PROCESS_NUM_MAX,
    };
    static constexpr unsigned int PROCESS_MAX = (unsigned int)Process::PROCESS_NUM_MAX;

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
    std::unique_ptr<UIRenderer>       mpUIRenderer;
    std::unique_ptr<TextureHolder>    mpTextureHolder;
    std::unique_ptr<NewMeshRenderer>  mpMeshRenderer;

    std::unique_ptr<ComputeExecuter>  mpComputeExecuter;

    std::unique_ptr<ForwardPass>      mpForwardPass;
    std::unique_ptr<ShadowPass>       mpShadowPass;
    std::unique_ptr<PostProcessPass>  mpPostProcessPass;
    std::unique_ptr<DeferredPass>     mpDefferedPass;
    std::unique_ptr<SSAOPass>         mpSSAOPass;
    std::unique_ptr<MakeCubeMapPass>  mpCubeMapPass;

    bool mbIsCastingShadow;
    bool mbIsDeffered;
    bool mbIsSSAO;
    bool mbIsCubeMap;

    u_int mCurrentPass;
    unsigned int mCurrentScreenNo;

    float mFrameTimer = .0f;

public:
    GameSystem();
    ~GameSystem();

    void Initialize(std::unique_ptr<GraphicsEngine>& p_graphics);
    void Update(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
    void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
    void RenderUI(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);

private:
    void RenderUIForSettings(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
    void RenderUIByRenderPasses(std::unique_ptr<GraphicsEngine>& p_graphics);
    void RenderUIForMainMenuBar(std::unique_ptr<GraphicsEngine>& p_graphics);
    void RenderUIForScene(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);

public:
    void LoadScene();
    void LoadNextScene(std::unique_ptr<GraphicsEngine>& p_graphics);
    void LoadRenderPasses(std::unique_ptr<GraphicsEngine>& p_graphics);
    void CheckShaderActivated();


    void SetRendererSettings(const Settings::Renderer& settings);
    void SetCurrentWindowSize(D3D::DevicePtr& p_device, unsigned int width, unsigned int height);

    std::unique_ptr<ActorManager>&      GetActorManagerPtr() { return mpActorManager; }
    std::unique_ptr<CameraController>&  GetCameraPtr()       { return mpCamera; }
    std::unique_ptr<LightController>&   GetLightPtr()        { return mpLight; }
    RenderPass*                         GetRenderPass(unsigned int render_pass_id);
    std::unique_ptr<NewMeshRenderer>&   GetMeshRenderer() { return mpMeshRenderer; }
    std::unique_ptr<UIRenderer>&        GetUIRenderer()     { return mpUIRenderer; }
    std::unique_ptr<TextureHolder>&     GetTextureHolderPtr() { return mpTextureHolder; }
    std::unique_ptr<ComputeExecuter>& GetComputeExecuter() { return mpComputeExecuter; }
    bool IsSSAOActivated() { return mbIsSSAO; }
    bool IsDefferedRendering() { return mbIsDeffered; }
};

#define  ENGINE Singleton<GameSystem>::Get()
 