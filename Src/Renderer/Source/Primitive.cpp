#include "Primitive.h"

#include <sstream>
#include <memory>
#include "./Utilities/misc.h"
#include "ResourceManager.h"
#include "Shader.h"


// TODO implement shader corresponding to PrimitiveBatch

using namespace DirectX;

Primitive::Primitive(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
    HRESULT hr = S_OK;
    Vertex vertices[] =
    {
        { XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT4(0, 0, 0, 0)},
        { XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT4(0, 0, 0, 0) },
    };

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.MiscFlags = 0;
    vbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA srData = {};
    srData.pSysMem = vertices;
    srData.SysMemPitch = 0;
    srData.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&vbDesc, &srData, mpVertexBuffer.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));



    D3D11_RASTERIZER_DESC rDesc = {};
    rDesc.AntialiasedLineEnable = FALSE;
    rDesc.FillMode = D3D11_FILL_SOLID;
    rDesc.CullMode = D3D11_CULL_NONE;
    rDesc.FrontCounterClockwise = FALSE;
    rDesc.DepthBias = 0;
    rDesc.DepthBiasClamp = 0;
    rDesc.SlopeScaledDepthBias = 0;
    rDesc.DepthClipEnable = FALSE;
    rDesc.ScissorEnable = FALSE;
    rDesc.MultisampleEnable = FALSE;
    hr = device->CreateRasterizerState(&rDesc, mRasterizerState.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = FALSE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.StencilEnable = FALSE;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    device->CreateDepthStencilState(&dsDesc, mDepthStencilState.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
}

Primitive::~Primitive()
{
}


void Primitive::Render(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context, 
    DirectX::XMFLOAT2 &pos, DirectX::XMFLOAT2 &size, 
    float angle,
    DirectX::XMFLOAT4 &color,
    std::shared_ptr<Shader>& shader,
    bool isSolid)
{
    HRESULT hr = S_OK;
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    D3D11_VIEWPORT viewport;
    UINT numViewport = 1;
    imm_context->RSGetViewports(&numViewport, &viewport);

    Vertex vertices[] = {
        { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0, 0, 1), color },
        { XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0, 0, 1), color },
        { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0, 0, 1), color },
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0, 0, 1), color },
    };

    angle *= 3.14f / 180.0f;

    float sinValue = sinf(angle);
    float cosValue = cosf(angle);

    for (auto i = 0; i < 4; ++i)
    {
        vertices[i].pos.x *= size.x;
        vertices[i].pos.y *= size.y;

        float rx = vertices[i].pos.x;
        float ry = vertices[i].pos.y;
        vertices[i].pos.x = cosValue * rx - sinValue * ry;
        vertices[i].pos.y = sinValue * rx + cosValue * ry;

        vertices[i].pos.x += pos.x;
        vertices[i].pos.y += pos.y;

        vertices[i].pos.x = -1.0f + vertices[i].pos.x * 2.0f / viewport.Width;
        vertices[i].pos.y = 1.0f - vertices[i].pos.y * 2.0f / viewport.Height;
    }

    D3D11_MAPPED_SUBRESOURCE mrData = {};

    imm_context->Map(mpVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &mrData);
    memcpy(mrData.pData, vertices, sizeof(vertices));
    imm_context->Unmap(mpVertexBuffer.Get(), 0);

    imm_context->IASetVertexBuffers(0, 1, mpVertexBuffer.GetAddressOf(), &stride, &offset);
    imm_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    imm_context->RSSetState(mRasterizerState.Get());

    shader->ActivateShaders(imm_context);
    imm_context->OMSetDepthStencilState(mDepthStencilState.Get(), 1);


    imm_context->Draw(4, 0);
}



PrimitiveBatch::PrimitiveBatch(Microsoft::WRL::ComPtr<ID3D11Device>& device, UINT numInstances):MAX_INSTANCES(numInstances)
{
    HRESULT hr = S_OK;

    Vertex vertices[] = {
        { XMFLOAT3(0, 0, 0) },
        { XMFLOAT3(1, 0, 0) },
        { XMFLOAT3(0, 1, 0) },
        { XMFLOAT3(1, 1, 0) },
    };

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.CPUAccessFlags = 0;
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.MiscFlags = 0;
    vbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA srData = {};
    srData.pSysMem = vertices;
    srData.SysMemPitch = 0;
    srData.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&vbDesc, &srData, mpVertexBuffer.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    D3D11_INPUT_ELEMENT_DESC layouts[] = {
        { "POSITION",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NDC_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "COLOR",         0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };


    ResourceManager::CreateVSFromFile(device, "./Data/Shaders/primitive_batch_vs.cso", mVertexShader.GetAddressOf(), mInputLayout.GetAddressOf(), layouts, ARRAYSIZE(layouts));
    ResourceManager::CreatePSFromFile(device, "./Data/Shaders/primitive_batch_ps.cso", mPixelShader.GetAddressOf());

    Instance* instances = new Instance[MAX_INSTANCES];
    {
        D3D11_BUFFER_DESC ibDesc = {};
        ibDesc.Usage = D3D11_USAGE_DYNAMIC;
        ibDesc.ByteWidth = sizeof(Instance) * MAX_INSTANCES;
        ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        ibDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        ibDesc.MiscFlags = 0;
        ibDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA srData = {};
        srData.pSysMem = instances;
        srData.SysMemPitch = 0;
        srData.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&ibDesc, &srData, mInstanceBuffer.GetAddressOf());
        if (FAILED(hr)) return;
    }
    delete[] instances;





    D3D11_RASTERIZER_DESC rDesc = {};
    rDesc.AntialiasedLineEnable = FALSE;
    rDesc.FillMode = D3D11_FILL_SOLID;
    rDesc.CullMode = D3D11_CULL_NONE;
    rDesc.FrontCounterClockwise = FALSE;
    rDesc.DepthBias = 0;
    rDesc.DepthBiasClamp = 0;
    rDesc.SlopeScaledDepthBias = 0;
    rDesc.DepthClipEnable = FALSE;
    rDesc.ScissorEnable = FALSE;
    rDesc.MultisampleEnable = FALSE;
    hr = device->CreateRasterizerState(&rDesc, mRasterizerState.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = FALSE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    device->CreateDepthStencilState(&depthStencilDesc, mDepthStencilState.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));



}

PrimitiveBatch::~PrimitiveBatch()
{
}

void PrimitiveBatch::Begin(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context)
{
    HRESULT hr = S_OK;
    UINT strides[2] = { sizeof(Vertex), sizeof(Instance) };
    UINT offsets[2] = { 0,0 };

    ID3D11Buffer* vbs[2] = { mpVertexBuffer.Get(), mInstanceBuffer.Get() };
    imm_context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
    imm_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    imm_context->IASetInputLayout(mInputLayout.Get());

    D3D11_RASTERIZER_DESC rDesc = {};
    rDesc.AntialiasedLineEnable = FALSE;
    rDesc.FillMode = D3D11_FILL_SOLID;
    rDesc.CullMode = D3D11_CULL_NONE;
    rDesc.FrontCounterClockwise = FALSE;
    rDesc.DepthBias = 0;
    rDesc.DepthBiasClamp = 0;
    rDesc.SlopeScaledDepthBias = 0;
    rDesc.DepthClipEnable = FALSE;
    rDesc.ScissorEnable = FALSE;
    rDesc.MultisampleEnable = FALSE;

    imm_context->RSSetState(mRasterizerState.Get());
    imm_context->VSSetShader(mVertexShader.Get(), nullptr, 0);
    imm_context->PSSetShader(mPixelShader.Get(), nullptr, 0);
    imm_context->OMSetDepthStencilState(mDepthStencilState.Get(), 1);

    UINT numViewport = 1;
    imm_context->RSGetViewports(&numViewport, &mViewport);

    D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
    D3D11_MAPPED_SUBRESOURCE mappedBuffer;
    hr = imm_context->Map(mInstanceBuffer.Get(), 0, map, 0, &mappedBuffer);
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    mInstances = static_cast<Instance*>(mappedBuffer.pData);

    mCountInstances = 0;



}

void PrimitiveBatch::DrawRect(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
    DirectX::XMFLOAT2 &pos,
    DirectX::XMFLOAT2 &size,
    float angle, 
    DirectX::XMFLOAT4 &color)
{
    _ASSERT_EXPR(mCountInstances < MAX_INSTANCES, L"Too Many Instances is EXISTING");

    float cx = size.x * 0.5f; float cy = size.y * 0.5f;
    DirectX::XMVECTOR scaling = DirectX::XMVectorSet(size.x, size.y, 1.0f, 0.0f);
    DirectX::XMVECTOR origin = DirectX::XMVectorSet(cx, cy, 0.0f, 0.0f);
    DirectX::XMVECTOR translation = DirectX::XMVectorSet(pos.x, pos.y, 0.0f, 0.0f);
    DirectX::XMMATRIX M = DirectX::XMMatrixAffineTransformation2D(scaling, origin, angle * 0.01745f, translation);
    DirectX::XMMATRIX N(
        2.0f / mViewport.Width, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f / mViewport.Height, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
        ); // •½s“Š‰e
    XMStoreFloat4x4(&mInstances[mCountInstances].ndc_transform, DirectX::XMMatrixTranspose(M * N));

    mInstances[mCountInstances].color = color;


    mCountInstances++;
}



void PrimitiveBatch::End(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context)
{
    imm_context->Unmap(mInstanceBuffer.Get(), 0);
    imm_context->DrawInstanced(4, mCountInstances, 0, 0);
}
