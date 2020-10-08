#pragma once
#include <stack>
#include <memory>
#include "SceneHelper.h"
#include "./Application/Helper.h"

class GraphicsEngine;
class Scene;
class SceneManager
{
public:


private:
    std::stack<std::unique_ptr<Scene>>    mSceneStack;
    SceneID                               mCurrentScene;
    SceneID                               mNextScene;
    bool                                  mClearFlag;

private:
    Microsoft::WRL::ComPtr<ID3D11Device>  m_pDevice;

public:
    SceneManager(Microsoft::WRL::ComPtr<ID3D11Device> device);
    ~SceneManager();


    void Execute(float elapsed_time);
    void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);




    void ChangeScene(const SceneID&, bool clearCurrentScene = false);
};