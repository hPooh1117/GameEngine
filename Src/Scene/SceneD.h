#pragma once
#include "Scene.h"
#include <memory>
#include <array>

#include "./Application/Helper.h"

#include "./Component/ComponentHelper.h"


class SceneD : public Scene
{
public:
    SceneD(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device);

    virtual void InitializeScene() override;

    virtual void Update(float elapsed_time) override;
    virtual void PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics) override;

    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;
    virtual void RenderUI() override;

    ~SceneD();

private:



    std::unique_ptr<AudioSystem> mpAudioSystem;
    std::unique_ptr<PhysicsManager> mpPhysics;
    std::unique_ptr<ParticleManager> mpParticleManager;
};