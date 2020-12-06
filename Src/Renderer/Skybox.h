#pragma once
#include "Mesh.h"
#include <string>
#include <array>
#include <algorithm>

class Texture;
class ComputedTexture;

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

    Microsoft::WRL::ComPtr<ID3D11Buffer>            mpVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            mpIndexBuffer;
    D3D::SamplerStatePtr                            mpClampSampler;
    bool mbIsDrawing = true;
    bool mbHasComputed = false;
public:
	Skybox();
    virtual ~Skybox();

    bool Initialize(D3D::DevicePtr& device,
        const wchar_t* filename = L"\0",
        float radius = BOX_SIZE);
    virtual void CreateBuffers(D3D::DevicePtr& device) override;
    virtual void Render(
        D3D::DeviceContextPtr& imm_context,
        float elapsed_time,
        const DirectX::XMMATRIX& world,
        CameraController* camera,
        Shader* shader,
        const MaterialData& mat_data,
        bool isShadow = false,
        bool isSolid = true
    ) override;
    void ConvertEquirectToCubeMap(D3D::DeviceContextPtr& imm_context);

    void RenderUI();

    ComputedTexture* GetTexturePtr() { return mpComputedTexs[mCurrentID].get(); }
    void SetCurrentSkybox(int id) { mCurrentID = std::clamp<int>(id, 0, SKYBOXID_MAX - 1); }

    bool HasComputed() { return mbHasComputed; }
};