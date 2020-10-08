#pragma once
#include "Scene.h"
#include <memory>
#include <DirectXMath.h>

#include "./Application/Helper.h"

class ShadowMap;
class MultiRenderTarget;

class SceneB : public Scene
{
public:
    SceneB(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device);

    virtual void Update(float elapsed_time);
    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);

    ~SceneB();
private:
    std::shared_ptr<Actor> m_pPlayer;
    std::shared_ptr<Actor> m_pCat;
    std::shared_ptr<Actor> m_pOswell;
    std::shared_ptr<Actor> m_pShiba;

    std::shared_ptr<Actor> m_pField;

private:
    std::unique_ptr<ShadowMap> m_pShadowMap;
    std::unique_ptr<MultiRenderTarget> m_pMRT;

    std::shared_ptr<Shader> m_pLambert;
    std::shared_ptr<Shader> m_pLambertForSkinnedMesh;
    std::shared_ptr<Shader> m_pLambertForSkinning;

    std::shared_ptr<Shader> m_pFromGBuffer;
    std::shared_ptr<Shader> m_pToGBuffer;
    std::shared_ptr<Shader> m_pToGBufferForSkinnedMesh;
    std::shared_ptr<Shader> m_pToGBufferForSkinning;

    std::shared_ptr<Shader> m_pToGBufferForBump;
    std::shared_ptr<Shader> m_pToGBufferSkinnedMeshForBump;
    std::shared_ptr<Shader> m_pDefferedLightShader;
};