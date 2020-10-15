#pragma once
#include "Scene.h"
#include <memory>
#include <array>

#include "./Application/Helper.h"

#include "./Component/ComponentHelper.h"

class SpriteBatch;

class SceneD : public Scene
{
public:
    SceneD(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device);

    virtual void InitializeScene() override;

    virtual void Update(float elapsed_time) override;
    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;

    ~SceneD();

private:

    std::shared_ptr<Actor> m_player;
    std::shared_ptr<Actor> m_non_player;
    std::shared_ptr<Actor> m_enemy;
    std::shared_ptr<Actor> m_field;
    std::shared_ptr<Actor> m_plane;
    std::array<std::shared_ptr<Actor>, 200> m_actors;

private:
    std::unique_ptr<SpriteBatch> mFont;
    std::shared_ptr<Shader> m_primitive_shader;
    std::shared_ptr<Shader> m_model_shader;
    std::shared_ptr<Shader> m_phong_shader;
    std::shared_ptr<Shader> m_line_shader;
    std::shared_ptr<Shader> m_plane_shader;
    std::shared_ptr<Shader> m_particle_shader;

    std::unique_ptr<AudioSystem> m_audio_system;
    std::unique_ptr<PhysicsManager> m_physics;
    std::unique_ptr<ParticleManager> m_particle_manager;
};