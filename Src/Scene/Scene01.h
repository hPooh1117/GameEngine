#pragma once
#include "Scene.h"
#include <memory>
#include <array>

#include "./Application/Helper.h"

class Sprite;

class Scene01 : public Scene
{
public:
    Scene01(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device);

    virtual void Update(float elapsed_time);
    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);

    
    ~Scene01();

private:
    std::unique_ptr<Sprite> mFont;
    
    std::unique_ptr<PhysicsManager> m_pPhysicsManager;


    bool mIsSolid;

    std::shared_ptr<Actor> m_pPlayer;
    std::array<std::shared_ptr<Actor>, 8> m_pTargets;
    std::shared_ptr<Actor> m_pTarget;

    std::shared_ptr<Actor> m_pEnemy;
    std::shared_ptr<Actor> m_pField;
    std::shared_ptr<Actor> m_pGrid;

    std::shared_ptr<Shader> m_pLambert;
    std::shared_ptr<Shader> m_pPhongShader;
    std::shared_ptr<Shader> m_pPhongSkinned;
    std::shared_ptr<Shader> m_pLineShader;
    std::shared_ptr<Shader> m_pSpriteShader;
};