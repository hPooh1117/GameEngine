#pragma once
#include "SceneManager.h"
#include "SceneHelper.h"

#include "./Application/Helper.h"
#include "./Application/Input.h"

#include "./Renderer/D3D_Helper.h"
#include "./Renderer/GraphicsDevice.h"

#include "./Utilities/Vector.h"

namespace Settings { struct Renderer; }



class Scene
{
public:
    static const Settings::Renderer mSettings[static_cast<UINT>(SceneID::SCENE_NUM_MAX)];
protected:
    SceneManager*                                 m_pManager;
    int                                           mTimer;
    int                                           mState;
    bool                                          mChangeFlag;
    float                                         mFadeoutTimer; 
    SceneID                                       mCurrentScene;
    SceneID                                       mNextScene;
    std::unordered_map<unsigned int, std::string> mSceneNameTable;
    //std::unique_ptr<Skybox>                       m_pSkyBox;
    //std::unique_ptr<Blender>                      m_pBlender;
    //std::unique_ptr<ActorManager>                 m_pActorManager;
    //std::shared_ptr<MeshRenderer>                 m_pRenderer;
    //std::shared_ptr<UIRenderer>                   m_pUIRenderer;
    //std::shared_ptr<CameraController>             m_pCameraController;
    //std::shared_ptr<LightController>              m_pLightController;



public:
    Scene(SceneManager* manager,
        Graphics::GraphicsDevice* p_device);
    //Scene(D3D::DevicePtr& device);
    virtual ~Scene() = default;

    virtual void InitializeScene() = 0;
    virtual void Update(float elapsed_time) = 0;
    virtual void PreCompute(Graphics::GraphicsDevice* p_graphics) = 0;
    virtual void Render(Graphics::GraphicsDevice* p_graphics, float elapsed_time) = 0;
    virtual void RenderUI() = 0;
    
    const std::string& GetCurrentSceneName();
    const std::string& GetSceneName(unsigned int num);

    void SetCurrntSceneID(SceneID id);
    void SetNextSceneID(SceneID id);
    void ToggleChangeFlag() { mChangeFlag = !mChangeFlag; }
    void ChangeScene();
};