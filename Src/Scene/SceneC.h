#pragma once
#include "Scene.h"
#include <memory>
#include <DirectXMath.h>

#include "./Application/Helper.h"

class ShadowMap;
class MultiRenderTarget;
class LightController;

class SceneC : public Scene
{
public:
    SceneC(SceneManager* manager, Microsoft::WRL::ComPtr<ID3D11Device>& device);

    virtual void Update(float elapsed_time);
    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);

    ~SceneC();
private:
    std::shared_ptr<Actor> m_pPlayer;
    std::shared_ptr<Actor> m_pCat;
    std::shared_ptr<Actor> m_pOswell;
    std::shared_ptr<Actor> m_pShiba;
    std::shared_ptr<Actor> m_pSphere;
    std::shared_ptr<Actor> m_pField;
    std::shared_ptr<Actor> m_pSky;

private:
    std::shared_ptr<Shader> m_pLambert;
    std::shared_ptr<Shader> m_pLambertForSkinnedMesh;
    std::shared_ptr<Shader> m_pLambertForSkinning;

    std::shared_ptr<Shader> m_pPointLight;
    std::shared_ptr<Shader> m_pPointLightForSkinnedMesh;
    std::shared_ptr<Shader> m_pPointLightForSkinning;
    std::shared_ptr<Shader> m_pPointLightBump;
    std::shared_ptr<Shader> m_pPointLightForSkinnedMeshBump;
    std::shared_ptr<Shader> m_pPointLightForOcean;

    std::shared_ptr<Shader> m_pFromGBuffer;
    std::shared_ptr<Shader> m_pToGBuffer;
    std::shared_ptr<Shader> m_pToGBufferForSkinnedMesh;
    std::shared_ptr<Shader> m_pToGBufferForSkinning;

    std::shared_ptr<Shader> m_pToGBufferForBump;
    std::shared_ptr<Shader> m_pToGBufferSkinnedMeshForBump;
    std::shared_ptr<Shader> m_pDefferedLightShader;

    std::shared_ptr<Shader> mSkyShader;
    std::shared_ptr<Shader> m_pFurShader;
    std::shared_ptr<Shader> m_pTesselation;
    std::shared_ptr<Shader> m_pTesselationForSkinnedMesh;
    std::shared_ptr<Shader> m_pDisplacementMapping;
};