#pragma once
#include "Scene.h"
#include <memory>
#include <array>

#include "./Application/Helper.h"
class Sprite;

class SceneTitle : public Scene
{
public:
    SceneTitle(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device);

    virtual void Update(float elapsed_time);
    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);

    ~SceneTitle() = default;

private:
    std::unique_ptr<Sprite> mFont;

    std::unique_ptr<PhysicsManager> m_pPhysicsManager;


    bool mIsSolid;

    std::shared_ptr<Actor> m_pPlayer;
    std::shared_ptr<Actor> m_pTarget;
    std::shared_ptr<Actor> m_pField;

    std::shared_ptr<Shader> m_pLambert;
    std::shared_ptr<Shader> m_pPhongShader;
    std::shared_ptr<Shader> m_pPhongSkinned;
    std::shared_ptr<Shader> m_pLineShader;
    std::shared_ptr<Shader> m_pSpriteShader;
};