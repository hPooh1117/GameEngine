#pragma once
#include <d3d11.h>
#include <wrl\client.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>

class Camera;
class Light;
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
    };

    struct CBufferForMesh
    {
        DirectX::XMFLOAT4X4 m_WVP;   // world * view * projection matrix
        DirectX::XMFLOAT4X4 m_world;           // world transformation matrix
        DirectX::XMFLOAT4 m_mat_color = { 1, 1, 1, 1 };

    };


protected:
    std::vector<Vertex> mVertices;
    std::vector<u_int> mIndices;


protected:
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pConstantBufferMesh   = nullptr;

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
        const std::shared_ptr<CameraController>& camera,
        const std::shared_ptr<Shader>& shader,
        const DirectX::XMFLOAT4& mat_color = DirectX::XMFLOAT4(1, 1, 1, 1),
        bool isShadow = false,
        bool isSolid = true
    ) = 0;

    inline void SetTopology(D3D11_PRIMITIVE_TOPOLOGY type) { mTopologyType = type; }
};