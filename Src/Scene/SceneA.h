#pragma once
#include "Scene.h"
#include <memory>

#include "./Application/Helper.h"

class Sprite;

class SceneA : public Scene
{
public:
    SceneA(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device);

    virtual void InitializeScene() override;
    virtual void Update(float elapsed_time) override;
    virtual void PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics) override;
    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;
    virtual void RenderUI() override;

    ~SceneA();


};