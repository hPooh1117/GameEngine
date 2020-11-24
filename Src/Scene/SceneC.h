#pragma once
#include "Scene.h"
#include <memory>
#include <DirectXMath.h>

#include "./Application/Helper.h"

class ShadowMap;
class MultiRenderTarget;
class LightController;

class SceneC : public Scene
{
public:
    SceneC(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device);

    virtual void InitializeScene() override;

    virtual void Update(float elapsed_time) override;
    virtual void PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics) override;

    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;

    ~SceneC();
private:
    std::shared_ptr<Actor> mpPlayer;
    std::shared_ptr<Actor> mpCat;
    std::shared_ptr<Actor> mpOswell;
    std::shared_ptr<Actor> mpShiba;
    std::shared_ptr<Actor> mpSphere;
    std::shared_ptr<Actor> mpField;
    std::shared_ptr<Actor> mpSky;
};