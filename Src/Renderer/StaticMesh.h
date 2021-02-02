#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include <string>
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "./Engine/CameraController.h"


class StaticMesh : public Mesh
{
public:
    struct Subset
    {
        std::wstring usemtl;
        uint32_t index_start = 0;
        uint32_t index_count = 0;
    };
    std::vector<Subset> mSubsets;

    struct MaterialData
    {
        std::wstring newmtl;
        DirectX::XMFLOAT3 Ka = { 0.2f, 0.2f, 0.2f };
        DirectX::XMFLOAT3 Kd = { 0.8f, 0.8f, 0.8f };
        DirectX::XMFLOAT3 Ks = { 1.0f, 1.0f, 1.0f };
        uint32_t illum = 1;
        std::wstring map_Kd;
        std::unique_ptr<Texture> texture;
    };

private:
    std::vector<MaterialData> mMaterials;



private:
    std::unique_ptr<Graphics::GPUBuffer> mpVertexBuffer;
    std::unique_ptr<Graphics::GPUBuffer> mpIndexBuffer;

public:
    StaticMesh(Graphics::GraphicsDevice* p_device, const wchar_t*, bool isFlippingV = false);
    ~StaticMesh();
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
    void LoadOBJFile(Graphics::GraphicsDevice* p_device, const wchar_t*, bool);
    void LoadMTLFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t*, std::vector<std::wstring>&);
};