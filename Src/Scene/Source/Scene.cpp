#include "Scene.h"


#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/UIRenderer.h"
#include "./Engine/LightController.h"
#include "./Engine/MeshRenderer.h"
#include "./Engine/Settings.h"

#include "./Renderer/Blender.h"
#include "./Renderer/Shader.h"
#include "./Renderer/Blender.h"
#include "./Renderer/Skybox.h"

#include "./Utilities/Log.h"


const Settings::Renderer Scene::mSettings[7] = {
    //shadow     ssao     deffered     cubemap
    { false,     false,    false,     false },
    { false,     false,    true,      false },
    { false,     false,    false,     false },
    { false,     false,    false,     false },
    { true,      true,     true,      false },
    { false,     false,    false,     true  },
    { false,     false,    false,     false },
};

Scene::Scene(SceneManager* manager,
    Graphics::GraphicsDevice* p_device)
    :mTimer(0),
    mState(0),
    mChangeFlag(false),
    mFadeoutTimer(0.0f),
    mCurrentScene(SceneID::SCENE_A),
    mNextScene(SceneID::SCENE_A),
    m_pManager(manager)
{
    //mSceneNameTable[static_cast<unsigned int>(SceneID::SCENETITLE)] = "Title";
    //mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE01)] = "Scene01";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_A)] = "Forward Shader ";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_B)] = "Deferred Shader";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_C)] = "SporLight Sea  ";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_D)] = "Physics Test   ";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_E)] = "SSAO Test      ";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_F)] = "Cubemap Test   ";
    mSceneNameTable[static_cast<unsigned int>(SceneID::SCENE_TEST)] = "PBR Test       ";

}


const std::string& Scene::GetCurrentSceneName()
{
    return mSceneNameTable.find(static_cast<unsigned int>(mCurrentScene))->second;
}
const std::string& Scene::GetSceneName(unsigned int num)
{
    if (num >= static_cast<unsigned int>(SceneID::SCENE_NUM_MAX)) Log::Warning("[SCENE MANAGER] Given Number is invalid.");
    return mSceneNameTable.find(num)->second;
}

void Scene::SetCurrntSceneID(SceneID id)
{
    mCurrentScene = id;
}

void Scene::SetNextSceneID(SceneID id)
{
    mNextScene = id;
}

void Scene::ChangeScene()
{
    if (mChangeFlag)
    {
        SceneID next = mNextScene;
        m_pManager->ChangeScene(next, true);
    }
}
