#pragma once
#include "Scene.h"
#include <memory>
#include <DirectXMath.h>

#include "./Application/Helper.h"

class ShadowMap;
class MultiRenderTarget;

struct SimpleBuffer
{
    int i;
    float f;
};

class SceneB : public Scene
{
public:
    SceneB(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device);


    virtual void InitializeScene() override;
    virtual void Update(float elapsed_time) override;
    virtual void PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics) override;

    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;
    virtual void RenderUI() override;

    ~SceneB();
private:
    std::shared_ptr<Actor> m_pPlayer;
    std::shared_ptr<Actor> m_pCat;
    std::shared_ptr<Actor> m_pOswell;
    std::shared_ptr<Actor> m_pShiba;

   

    std::shared_ptr<Actor> m_pField;


};