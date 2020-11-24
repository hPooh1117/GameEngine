#pragma once
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "D3D_Helper.h"
#include "./Engine/DataStructures.h"

class Shader;
class CameraController;


class Mesh
{
public:
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT3 tangent;
        DirectX::XMFLOAT3 binormal;
    };

    struct CBufferForMesh
    {
        DirectX::XMFLOAT4X4 WVP;   // world * view * projection matrix
        DirectX::XMFLOAT4X4 world;           // world transformation matrix
        DirectX::XMFLOAT4X4 invProj;
    };
    struct CBufferForMaterial
    {
        DirectX::XMFLOAT4 mat_color = { 1, 1, 1, 1 };
        DirectX::XMFLOAT3 specularColor;
        float metalness;
        float roughness;
        float diffuse = 0.0f;
        float specular = 0.0f;
        int   textureConfig;
    };

protected:
    std::vector<Vertex> mVertices;
    std::vector<u_int> mIndices;


protected:
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pConstantBufferMesh   = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pConstantBufferMaterial = nullptr;

    // RenderState
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_pRasterizerWire       = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_pRasterizerSolid      = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY                        mTopologyType         = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

public:

    Mesh() {}
	virtual ~Mesh() {}

	virtual void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device) = 0;

	virtual void Render(
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
        float elapse_time,
        const DirectX::XMMATRIX& world,
        CameraController* camera,
        Shader* shader,
        const MaterialData& mat_data,
        bool isShadow = false,
        bool isSolid = true
    ) = 0;

    inline void SetTopology(D3D11_PRIMITIVE_TOPOLOGY type) { mTopologyType = type; }
};