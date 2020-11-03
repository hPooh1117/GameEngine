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
    std::shared_ptr<Actor> mpSphere;
    std::shared_ptr<Actor> mpEarth;
    std::shared_ptr<Actor> mpToon;
    std::shared_ptr<Actor> m_pPBRTest;
    std::shared_ptr<Actor> m_field;
    std::shared_ptr<Actor> mpGrid;

};