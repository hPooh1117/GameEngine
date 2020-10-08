#include "Scene.h"
#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/LightController.h"
#include "./Engine/MeshRenderer.h"

#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"
#include "./Renderer/GraphicsEngine.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Skybox.h"

Scene::Scene(SceneManager* manager,
    Microsoft::WRL::ComPtr<ID3D11Device>& device)
    :mTimer(0),
    mState(0),
    mChangeFlag(false),
    mFadeoutTimer(0.0f),
    mCurrentScene(SceneID::SCENE01),
    mNextScene(SceneID::SCENE01),
    m_pManager(manager),
    m_pActorManager(std::make_unique<ActorManager>()),
    m_pBlender(std::make_unique<Blender>(device)),
    m_pCameraController(std::make_shared<CameraController>()),
    m_pLightController(std::make_shared<LightController>(device)),
    m_pSkyBox(nullptr),
    m_pUIRenderer(std::make_shared<UIRenderer>())
{
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENETITLE)] = "Title";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE01)] = "Scene01";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_A)] = "SceneA";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_B)] = "SceneB";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_C)] = "SceneC";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_D)] = "SceneD";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_E)] = "SceneE";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_F)] = "SceneF";

    m_pRenderer = std::make_shared<MeshRenderer>(device, m_pCameraController);

};



const std::string& Scene::GetSceneName()
{
    return mSceneNameTable.find(static_cast<unsigned int>(mCurrentScene))->second;
}

void Scene::SetSceneID(SceneID id)
{
    mCurrentScene = id;
}

void Scene::ChangeScene()
{
    if (InputPtr->OnKeyTrigger("0"))
    {
        mChangeFlag = true;
    }

    if (mChangeFlag)
    {
        SceneID next = mNextScene;
        m_pManager->ChangeScene(next, true);
    }
}