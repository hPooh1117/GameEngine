#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include <string>
#include "Mesh.h"
class Texture;
class Shader;
// Unit11
class StaticMesh : public Mesh
{
public:
    struct Subset
    {
        std::wstring usemtl;
        u_int index_start = 0;
        u_int index_count = 0;
    };
    std::vector<Subset> mSubsets;

    struct Material
    {
        std::wstring newmtl;
        DirectX::XMFLOAT3 Ka = { 0.2f, 0.2f, 0.2f };
        DirectX::XMFLOAT3 Kd = { 0.8f, 0.8f, 0.8f };
        DirectX::XMFLOAT3 Ks = { 1.0f, 1.0f, 1.0f };
        u_int illum = 1;
        std::wstring map_Kd;
        std::shared_ptr<Texture> texture;
    };

private:
    //std::vector<Vertex>   m_vertices;
    //std::vector<u_int>    m_indices;
    std::vector<Material> mMaterials;



private:
    // Buffers
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>      mSamplerState;
    D3D11_TEXTURE2D_DESC mTexture2DDesc;

public:
    StaticMesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t*, bool isFlippingV = false);
    ~StaticMesh();
    virtual void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;
    virtual void Render(
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
        float elapsed_time,
        const DirectX::XMMATRIX& world,
        const std::shared_ptr<CameraController>& camera,
        const std::shared_ptr<Shader>& shader,
        const DirectX::XMFLOAT4& mat_color = DirectX::XMFLOAT4(1, 1, 1, 1),
        bool isShadow = false,
        bool isSolid = true
    ) override;
    void LoadOBJFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t*, bool);
    void LoadMTLFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t*, std::vector<std::wstring>&);
};