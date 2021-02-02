#pragma once
#include "Scene.h"
#include <memory>
#include <array>

#include "./Application/Helper.h"

#include "./Component/ComponentHelper.h"


class SceneD : public Scene
{
public:
    SceneD(SceneManager* manager, Graphics::GraphicsDevice* p_device);

    virtual void InitializeScene() override;

    virtual void Update(float elapsed_time) override;
    virtual void PreCompute(Graphics::GraphicsDevice* p_graphics) override;

    virtual void Render(Graphics::GraphicsDevice* p_graphics, float elapsed_time) override;
    virtual void RenderUI() override;

    ~SceneD();

private:



    std::unique_ptr<AudioSystem> mpAudioSystem;
    std::unique_ptr<PhysicsManager> mpPhysics;
    std::unique_ptr<ParticleManager> mpParticleManager;
};