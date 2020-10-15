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
    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;

    ~SceneA();

private:
    std::unique_ptr<Sprite> mFont;
    
    bool mIsSolid;

    std::shared_ptr<Actor> m_pSphere;
    std::shared_ptr<Actor> m_pEarth;
    std::shared_ptr<Actor> m_pToon;
    std::shared_ptr<Actor> m_pPBRTest;
    std::shared_ptr<Actor> m_field;
    std::shared_ptr<Actor> mGrid;

    std::shared_ptr<Shader> m_pPhong;
    std::shared_ptr<Shader> m_env_map_shader;
    std::shared_ptr<Shader> m_line_shader;
    std::shared_ptr<Shader> m_sky_shader;
    std::shared_ptr<Shader> m_fog_shader;
    std::shared_ptr<Shader> m_bump_shader;
    std::shared_ptr<Shader> m_toon_shader;
    std::shared_ptr<Shader> m_pSpriteShader;
    
    std::shared_ptr<Shader> m_pForwardBRDF;

};