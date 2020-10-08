#pragma once
#include "SceneManager.h"

#include "./Application/Helper.h"
#include "./Application/Input.h"

#include "./Utilities/Vector.h"

class GraphicsEngine;


class Scene
{
protected:
    SceneManager*                                 m_pManager;
    int                                           mTimer;
    int                                           mState;
    bool                                          mChangeFlag;
    float                                         mFadeoutTimer; 
    SceneID                                       mCurrentScene;
    SceneID                                       mNextScene;
    std::unordered_map<unsigned int, std::string> mSceneNameTable;

    std::unique_ptr<SkyBox>                       m_pSkyBox;
    std::unique_ptr<Blender>                      m_pBlender;
    std::unique_ptr<ActorManager>                 m_pActorManager;
    std::shared_ptr<MeshRenderer>                 m_pRenderer;
    std::shared_ptr<UIRenderer>                   m_pUIRenderer;
    std::shared_ptr<CameraController>             m_pCameraController;
    std::shared_ptr<LightController>              m_pLightController;



public:
    Scene(SceneManager* manager,
        Microsoft::WRL::ComPtr<ID3D11Device>& device);
    virtual ~Scene() = default;

    virtual void Update(float elapsed_time) = 0;
    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) = 0;
    
    const std::string& GetSceneName();

    void SetSceneID(SceneID id);
    void ChangeScene();
};