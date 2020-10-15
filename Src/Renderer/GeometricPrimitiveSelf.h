#pragma once
#include <DirectXMath.h>
#include "D3D_Helper.h"
#include <memory>
#include <vector>
#include "Mesh.h"
class Shader;
class Light;
class Texture;

class GeometricPrimitiveSelf : public Mesh
{
protected:
    // Buffers
    D3D::BufferPtr            m_pVertexBuffer;
    D3D::BufferPtr            m_pIndexBuffer;


public:
    GeometricPrimitiveSelf(
        D3D::DevicePtr& device,
        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );
    virtual ~GeometricPrimitiveSelf();

    void CreateBuffers(D3D::DevicePtr& device) override;

    virtual void SetRenderState(D3D::DeviceContextPtr& imm_context) = 0;

    virtual void Render(
        D3D::DeviceContextPtr& imm_context,
        float elapsed_time,
        const DirectX::XMMATRIX& world,
        const std::shared_ptr<CameraController>& camera,
        const std::shared_ptr<Shader>& shader,
        const DirectX::XMFLOAT4& mat_color = DirectX::XMFLOAT4(1, 1, 1, 1),
        bool isShadow = false,
        bool isSolid = true
    ) override;
};

// if you use Basic Line, you must choose shader "line.hlsl".
class BasicLine : public GeometricPrimitiveSelf
{
private:
    struct CBufferForLine
    {
        DirectX::XMFLOAT4 param = {1, 0, 0, 0};
    };
   D3D::BufferPtr mConstantBufferForLine;
public:
    BasicLine(D3D::DevicePtr& device);
    virtual void SetRenderState(D3D::DeviceContextPtr& imm_context) override;
};



class BasicCube : public GeometricPrimitiveSelf
{
private:
    std::unique_ptr<Texture> mTexture;

public:
    BasicCube(D3D::DevicePtr& device, const wchar_t* filename);
    bool LoadTexture(
        D3D::DevicePtr& device,
        const wchar_t* filename);

    virtual void SetRenderState(D3D::DeviceContextPtr& imm_context) override;

};



class BasicCylinder : public GeometricPrimitiveSelf
{
public:
    BasicCylinder(
        D3D::DevicePtr& device,
        unsigned int slices = 8);
    virtual void SetRenderState(D3D::DeviceContextPtr& imm_context) override;
};



class BasicSphere : public GeometricPrimitiveSelf
{
private:
    std::unique_ptr<Texture> mTexture;

public:
    BasicSphere(
        D3D::DevicePtr& device,
        const wchar_t* filename = L"\0",
        unsigned int slices = 8,
        unsigned int stacks = 8,
        float radius = 0.5f
    );
    bool LoadTexture(
        D3D::DevicePtr& device,
        const wchar_t* filename);
    virtual void SetRenderState(D3D::DeviceContextPtr& imm_context) override;

};



class BasicCapsule : public GeometricPrimitiveSelf
{
public:
    BasicCapsule(
        D3D::DevicePtr& device,
        unsigned int slices = 8,
        unsigned int stacks = 8,
        float radius = 0.5f,
        float height = 1.0f);
    virtual void SetRenderState(D3D::DeviceContextPtr& imm_context) override;

};