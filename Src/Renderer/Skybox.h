#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "ComputedTexture.h"
#include "Shader.h"

#include <string>
#include <array>
#include <algorithm>


enum SkyboxTextureID
{
    EFootprintCourt,
    EFactoryCatWalk,
    EMonoLake,
    ETokyoBigSight,
    EUenoShrine,
    EWalkOfFame,
    EDUMMY,
    EGray0,
    EGray1,
    SKYBOXID_MAX
};

class Skybox : public Mesh
{
private:
    static const wchar_t* SKYBOX_TEXTURE[SKYBOXID_MAX];
    static const std::wstring TEXTURE_PATH;
    static constexpr float BOX_SIZE = 1.0f;

    unsigned int mCurrentID;
    std::array<std::unique_ptr<Texture>, SKYBOXID_MAX> mpTextures;
    std::array<std::unique_ptr<ComputedTexture>, SKYBOXID_MAX> mpComputedTexs;
    std::unique_ptr<Shader> mpCS_Equirectangular2Cube;
    std::unique_ptr<Graphics::GPUBuffer> mpVertexBuffer;
    std::unique_ptr<Graphics::GPUBuffer> mpIndexBuffer;
    bool mbIsDrawing = true;
    bool mbHasComputed = false;
public:
	Skybox();
    virtual ~Skybox();

    bool Initialize(Graphics::GraphicsDevice* p_device,
        const wchar_t* filename = L"\0",
        float radius = BOX_SIZE);
    virtual void CreateBuffers(Graphics::GraphicsDevice* p_device) override;
    virtual void Render(
        Graphics::GraphicsDevice* p_device,
        float elapsed_time,
        const DirectX::XMMATRIX& world,
        CameraController* camera,
        Shader* shader,
        const Material& mat_data,
        bool isShadow = false,
        bool isSolid = true
    ) override;
    void ConvertEquirectToCubeMap(Graphics::GraphicsDevice* p_device);

    void RenderUI();

    ComputedTexture* GetTexturePtr() { return mpComputedTexs[mCurrentID].get(); }
    void SetCurrentSkybox(int id) { mCurrentID = std::clamp<int>(id, 0, SKYBOXID_MAX - 1); }

    bool HasComputed() { return mbHasComputed; }
};