#pragma once
#include "Scene.h"

class GraphicsEngine;
class ComputedTexture;
class Texture;

class SceneTest :
    public Scene
{
public:
    SceneTest(SceneManager* p_manager, Microsoft::WRL::ComPtr<ID3D11Device>& p_device);

    virtual void InitializeScene() override;
    virtual void Update(float elapsed_time) override;
    virtual void PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics) override;

    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;
    virtual void RenderUI() override;

    ~SceneTest();

private:
    static constexpr UINT PBR_EXHIBITION_SIZE = 25;

    enum ActorID
    {
        EPaintedMetal,
        EWoodedPlank,
        ECopperTiles,
        EBlackHerringBone,
        EBlueGreenHexagonal,
        EPBRSphere0,
        EPBRSphereParam = EPBRSphere0 + PBR_EXHIBITION_SIZE,
        EModelPBR,

        ACTOR_SIZE_MAX,
    };


    std::unique_ptr<ComputedTexture> mpIrradianceTex;
    std::unique_ptr<ComputedTexture> mpSpecularMapTex;
    std::unique_ptr<ComputedTexture> mpEnvironmentTex;
    std::unique_ptr<ComputedTexture> mpSpecularBRDF_LUT;
    std::unique_ptr<Texture>         mpSkyTex;

    int  mPreComputeState;

    bool mbAutoSetParameter;
};

