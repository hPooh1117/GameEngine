#pragma once

#include "Mesh.h"


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
        CameraController* camera,
        Shader* shader,
        const MaterialData& mat_data,
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
    void SetRenderState(D3D::DeviceContextPtr& imm_context);
};



class BasicCube : public GeometricPrimitiveSelf
{
public:
    BasicCube(D3D::DevicePtr& device);
    void SetRenderState(D3D::DeviceContextPtr& imm_context){}

};



class BasicCylinder : public GeometricPrimitiveSelf
{
public:
    BasicCylinder(
        D3D::DevicePtr& device,
        unsigned int slices = 8);
    void SetRenderState(D3D::DeviceContextPtr& imm_context) {}

};



class BasicSphere : public GeometricPrimitiveSelf
{
public:
    BasicSphere(
        D3D::DevicePtr& device,
        unsigned int slices = 8,
        unsigned int stacks = 8,
        float radius = 0.5f
    );
    void SetRenderState(D3D::DeviceContextPtr& imm_context) {}

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
    void SetRenderState(D3D::DeviceContextPtr& imm_context) {}

};