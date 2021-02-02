#pragma once

#include "Mesh.h"


class GeometricPrimitiveSelf : public Mesh
{
protected:
    // Buffers
    //D3D::BufferPtr            m_pVertexBuffer;
    //D3D::BufferPtr            m_pIndexBuffer;
    
    std::unique_ptr<Graphics::GPUBuffer> mpVertexBuffer;
    std::unique_ptr<Graphics::GPUBuffer> mpIndexBuffer;

    CBufferForMesh mMeshData;
public:
    GeometricPrimitiveSelf(
        Graphics::GraphicsDevice* p_device,
        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );
    virtual ~GeometricPrimitiveSelf();

    void CreateBuffers(Graphics::GraphicsDevice* p_device) override;
    virtual void SetRenderState(Graphics::GraphicsDevice* p_device) = 0;

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
    BasicLine(Graphics::GraphicsDevice* p_device);
    void SetRenderState(Graphics::GraphicsDevice* p_device);
};



class BasicCube : public GeometricPrimitiveSelf
{
public:
    BasicCube(Graphics::GraphicsDevice* p_device);
    void SetRenderState(Graphics::GraphicsDevice* p_device){}

};



class BasicCylinder : public GeometricPrimitiveSelf
{
public:
    BasicCylinder(
        Graphics::GraphicsDevice* p_device,
        unsigned int slices = 8);
    void SetRenderState(Graphics::GraphicsDevice* p_device) {}

};



class BasicSphere : public GeometricPrimitiveSelf
{
public:
    BasicSphere(
        Graphics::GraphicsDevice* p_device,
        unsigned int slices = 8,
        unsigned int stacks = 8,
        float radius = 0.5f
    );
    void SetRenderState(Graphics::GraphicsDevice* p_device) {}

};



class BasicCapsule : public GeometricPrimitiveSelf
{
public:
    BasicCapsule(
        Graphics::GraphicsDevice* p_device,
        unsigned int slices = 8,
        unsigned int stacks = 8,
        float radius = 0.5f,
        float height = 1.0f);
    void SetRenderState(Graphics::GraphicsDevice* p_device) {}

};