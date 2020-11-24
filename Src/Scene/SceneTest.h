#pragma once
#include "Scene.h"

class GraphicsEngine;
class ComputedTexture;
class NewTexture;

class SceneTest :
    public Scene
{
public:
    SceneTest(SceneManager* p_manager, Microsoft::WRL::ComPtr<ID3D11Device>& p_device);

    virtual void InitializeScene() override;
    virtual void Update(float elapsed_time) override;
    virtual void PreCompute(std::unique_ptr<GraphicsEngine>& p_graphics) override;

    virtual void Render(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time) override;

    ~SceneTest();

private:
    std::shared_ptr<Actor> mpPaintedMetal;
    std::shared_ptr<Actor> mpWoodedPlank;
    std::shared_ptr<Actor> mpCopperTiles;
    std::shared_ptr<Actor> mpBlackHerringBoneTile;
    std::shared_ptr<Actor> mpBlurGreenHexagonalTile;
    std::shared_ptr<Actor> mpActors[25];

    std::shared_ptr<Actor> mpPBR;

    std::unique_ptr<ComputedTexture> mpIrradianceTex;
    std::unique_ptr<ComputedTexture> mpSpecularMapTex;
    std::unique_ptr<ComputedTexture> mpEnvironmentTex;
    std::unique_ptr<ComputedTexture> mpSpecularBRDF_LUT;
    std::unique_ptr<NewTexture>      mpSkyTex;

    bool mbAutoSetParameter;
};

