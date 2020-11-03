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
    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;

    ~SceneD();

private:

    std::shared_ptr<Actor> mpPlayer;
    std::shared_ptr<Actor> mpNonPlayer;
    std::shared_ptr<Actor> mpEnemy;
    std::shared_ptr<Actor> mpField;
    std::shared_ptr<Actor> mpPlane;
    std::array<std::shared_ptr<Actor>, 200> mpActors;


    std::unique_ptr<AudioSystem> mpAudioSystem;
    std::unique_ptr<PhysicsManager> mpPhysics;
    std::unique_ptr<ParticleManager> mpParticleManager;
};