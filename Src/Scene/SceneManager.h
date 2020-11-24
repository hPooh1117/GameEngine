#pragma once
#include <stack>
#include <memory>
#include "SceneHelper.h"
#include "./Application/Helper.h"
#include "./Renderer/D3D_Helper.h"

class GraphicsEngine;
class Scene;

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
private:
    D3D::DevicePtr  m_pDevice;

public:
    SceneManager(D3D::DevicePtr& p_device);
    ~SceneManager();

    void InitializeLoadingScene();
    void InitializeCurrentScene();

    void ExecuteLoadingScene(float elapsed_time);
    void ExecuteCurrentScene(float elapsed_time);
    void LoadNextScene();

    void RenderLoadingScene(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
    void RenderCurrentScene(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
    void PreComputeForNextScene(std::unique_ptr<GraphicsEngine>& p_graphics);
    void RenderUI();

    bool IsLoading();


    void ChangeScene(const SceneID&, bool clearCurrentScene = false);
};