#pragma once
#include "Mesh.h"
#include <string>
#include <array>
#include <algorithm>

class Texture;
class Texture;

enum SkyboxTextureID
{
    EFootprintCourt,
    ERidgecrestRoad,
    ESerpentineValley,
    ETokyoBigSight,
    EUenoShrine,
    EWalkOfFame,

    ENUM_SKYBOXID_MAX
};

class Skybox : public Mesh
{
private:
    static const wchar_t* SKYBOX_TEXTURE[ENUM_SKYBOXID_MAX];
    static const std::wstring TEXTURE_PATH;

    unsigned int mCurrentID;
    std::array<std::unique_ptr<Texture>, ENUM_SKYBOXID_MAX> mpTextures;
    std::unique_ptr<Texture> mpTexture;

    Microsoft::WRL::ComPtr<ID3D11Buffer>            mpVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            mpIndexBuffer;
    D3D::SamplerStatePtr                            mpClampSampler;
    bool mbIsDrawing = true;
public:
	Skybox(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const wchar_t* filename = L"\0",
        unsigned int slices = 15,
        unsigned int stacks = 15,
        float radius = 1000.0f
    );

    virtual void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;
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

    void RenderUI();

    void SetCurrentSkybox(int id) { mCurrentID = std::clamp<int>(id, 0, ENUM_SKYBOXID_MAX - 1); }

    virtual ~Skybox();
};