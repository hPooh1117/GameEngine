#pragma once
#include <stack>
#include <memory>
#include "SceneHelper.h"
#include "./Application/Helper.h"
#include "./Renderer/D3D_Helper.h"

class GraphicsEngine;
class Scene;
namespace Settings{ struct Renderer; }

class SceneManager
{
public:


private:
    std::stack<std::unique_ptr<Scene>>    mSceneStack;
    std::unique_ptr<Scene>                m_pLoadingScene;
    SceneID                               mCurrentScene;
    SceneID                               mNextScene;
    bool                                  mClearFlag;
    bool                                  m_bIsLoading;
    bool                                  mbIsInitialized;
private:
    D3D::DevicePtr  m_pDevice;

public:
    SceneManager(D3D::DevicePtr& p_device);
    ~SceneManager();

    void CreateScene(SceneID scene_id);
    void InitializeLoadingScene();
    void InitializeCurrentScene();

    void ExecuteLoadingScene(float elapsed_time);
    void ExecuteCurrentScene(float elapsed_time);
    void LoadNextScene();

    void RenderLoadingScene(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
    void RenderCurrentScene(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
    void RenderUIForCurrentScene();
    void PreComputeForNextScene(std::unique_ptr<GraphicsEngine>& p_graphics);
    void RenderUI();

    bool IsLoading() { return m_bIsLoading; }
    bool IsInitialized() { return mbIsInitialized; }

    const Settings::Renderer& GetNextSceneSettings();

    void ChangeScene(const SceneID&, bool clearCurrentScene = false);
};