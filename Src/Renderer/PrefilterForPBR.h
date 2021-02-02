#pragma once
#include "ComputedTexture.h"
#include "Texture.h"
#include "GraphicsDevice.h"
#include "Shader.h"

#include <memory>

class PrefilterForPBR
{
public:
    enum CSType
    {
        EPreComputeIrradiance,
        EPreComputeSpecular,
        EEquirectangular2Cube,
        ESpecularBRDF,

        CSTYPE_COUNT,
    };

private:
    std::unique_ptr<ComputedTexture> mpIrradianceTex;
    std::unique_ptr<ComputedTexture> mpSpecularMapTex;
    std::unique_ptr<ComputedTexture> mpEnvironmentTex;
    std::unique_ptr<ComputedTexture> mpSpecularBRDF_LUT;
    std::unique_ptr<Texture>         mpSkyTex;
    std::unique_ptr<Shader>          mpShaders[CSTYPE_COUNT];
    int  mPreComputeState = 0;

public:
    PrefilterForPBR()
        :mpIrradianceTex(std::make_unique<ComputedTexture>()),
        mpSpecularMapTex(std::make_unique<ComputedTexture>()),
        mpEnvironmentTex(std::make_unique<ComputedTexture>()),
        mpSpecularBRDF_LUT(std::make_unique<ComputedTexture>()),
        mpSkyTex(std::make_unique<Texture>())
    {
        for (int i = 0; i < CSTYPE_COUNT; ++i)
        {
            mpShaders[i] = std::make_unique<Shader>();
        }
    }

    bool Initialize(Graphics::GraphicsDevice* p_device);
    void PreCompute(Graphics::GraphicsDevice* p_device);
    void SetModifiedTextures(Graphics::GraphicsDevice* p_device);
};