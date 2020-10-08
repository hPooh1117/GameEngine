#pragma once
#include "Mesh.h"

class Texture;
class SkyBox : public Mesh
{
private:
    std::unique_ptr<Texture> m_pTexture;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pIndexBuffer;

public:
	SkyBox(Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const wchar_t* filename = L"\0",
        unsigned int slices = 12,
        unsigned int stacks = 12,
        float radius = 50.0f
    );

    bool loadTex(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t* filename);
    virtual void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;
    virtual void Render(
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
        float elapse_time,
        const DirectX::XMMATRIX& world,
        const std::shared_ptr<CameraController>& camera,
        const std::shared_ptr<Shader>& shader,
        const DirectX::XMFLOAT4& mat_color = DirectX::XMFLOAT4(1, 1, 1, 1),
        bool isShadow = false,
        bool isSolid = true
    ) override;
    virtual ~SkyBox();
};