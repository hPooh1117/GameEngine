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

class RenderPass;
class ForwardPass;
class ShadowPass;
class PostProcessPass;
class DefferedPass;
class SSAOPass;

class GameSystem
{

public:
    static const unsigned int THREAD_SIZE;

public:
    static std::mutex                 mLoadingMutex;
    static unsigned int mCurrentWidth;
    static unsigned int mCurrentHeight;

private:                              
    std::unique_ptr<SceneManager>     m_pSceneManager;
    std::unique_ptr<PerfTimer>        mpTimer;
    std::shared_ptr<ThreadPool>       mpThreadPool;

    bool m_bIsLoading;

    std::unique_ptr<ActorManager>     mpActorManager;
    std::unique_ptr<CameraController> mpCamera;
    std::unique_ptr<LightController>  mpLight;
    std::unique_ptr<UIRenderer>       mpUIRenderer;
    std::unique_ptr<TextureHolder>    mpTextureHolder;
    std::unique_ptr<NewMeshRenderer>  mpMeshRenderer;

    std::unique_ptr<ForwardPass>      mpForwardPass;
    std::unique_ptr<ShadowPass>       mpShadowPass;
    std::unique_ptr<PostProcessPass>  mpPostProcessPass;
    std::unique_ptr<DefferedPass>     mpDefferedPass;
    std::unique_ptr<SSAOPass>         mpSSAOPass;

    bool mbIsCastingShadow;
    bool mbIsDeffered;
    bool mbIsSSAO;

    u_int mCurrentPass;
    unsigned int mCurrentScreenNo;

public:
    GameSystem();
    ~GameSystem();

    void Initialize(D3D::DevicePtr& p_device);
    void Update(D3D::DevicePtr& p_device, float elapsed_time);
    void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
    void RenderUIByRenderPasses();
    void RenderUI(std::unique_ptr<GraphicsEngine>& p_graphics);

    void LoadScene();

    void NotCastShadow() { mbIsCastingShadow = false; }
    void CastShadow() { mbIsCastingShadow = true; }
    void SetIsDefferedRendering(bool flag) { mbIsDeffered = flag; }
    void SetSSAO(bool flag) { mbIsSSAO = flag; }
    void SetCurrentWindowSize(D3D::DevicePtr& p_device, unsigned int width, unsigned int height);

    std::unique_ptr<ActorManager>&      GetActorManagerPtr() { return mpActorManager; }
    std::unique_ptr<CameraController>&  GetCameraPtr()       { return mpCamera; }
    std::unique_ptr<LightController>&   GetLightPtr()        { return mpLight; }
    RenderPass*                         GetRenderPass(unsigned int render_pass_id);
    std::unique_ptr<NewMeshRenderer>&   GetMeshRenderer() { return mpMeshRenderer; }
    std::unique_ptr<UIRenderer>&        GetUIRenderer()     { return mpUIRenderer; }
    std::unique_ptr<TextureHolder>&     GetTextureHolderPtr() { return mpTextureHolder; }
    bool GetSSAOActivate() { return mbIsSSAO; }
};

#define  ENGINE Singleton<GameSystem>::Get()