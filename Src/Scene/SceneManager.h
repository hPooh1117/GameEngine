#pragma once
#include <stack>
#include <memory>
#include "SceneHelper.h"
#include "./Application/Helper.h"
#include "./Renderer/D3D_Helper.h"
#include "./Renderer/GraphicsDevice.h"


class Scene;
namespace Settings{ struct Renderer; }

class SceneManager
{
public:


private:
    std::stack<std::unique_ptr<Scene>>    mSceneStack;
    std::unique_ptr<Scene>                mpLoadingScene;
    SceneID                               mCurrentScene;
    SceneID                               mNextScene;
    bool                                  mbClearFlag;
    bool                                  mbIsLoading;
    bool                                  mbIsInitialized;
private:
    Graphics::GraphicsDevice* mpGraphicsDevice;

public:
    SceneManager(Graphics::GraphicsDevice* p_device);
    ~SceneManager();

    void CreateScene(SceneID scene_id);
    void InitializeLoadingScene();
    void InitializeCurrentScene();

    void ExecuteLoadingScene(float elapsed_time);
    void ExecuteCurrentScene(float elapsed_time);
    void LoadNextScene();

    void RenderLoadingScene(Graphics::GraphicsDevice* device, float elapsed_time);
    void RenderCurrentScene(Graphics::GraphicsDevice* device, float elapsed_time);
    void RenderUIForCurrentScene();
    void PreComputeForNextScene(Graphics::GraphicsDevice* device);
    void RenderUI();

    bool IsLoading() { return mbIsLoading; }
    bool IsInitialized() { return mbIsInitialized; }

    const Settings::Renderer& GetNextSceneSettings();

    void ChangeScene(const SceneID&, bool clearCurrentScene = false);
};