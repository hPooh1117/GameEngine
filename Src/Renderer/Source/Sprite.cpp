#include "sprite.h"

#include "ResourceManager.h"
#include "./Utilities/misc.h"
#include "Texture.h"
#include "Shader.h"

//---< namespace >-----------------------------------------------------------------------------------------------------
using namespace DirectX;


//******************************************************************************
//
//      sprite
//
//******************************************************************************
Sprite::Sprite(D3D::DevicePtr& device)
{
    HRESULT hr = S_OK;

    m_pTexture = std::unique_ptr<Texture>();

    // 頂点情報のセット
    vertex vertices[] = {
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
    };


    // 頂点バッファ―オブジェクトの生成
    D3D11_BUFFER_DESC vbDesc;
    vbDesc.ByteWidth = sizeof(vertex) * 4;
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;        // accessable by GPU(read only) and CPU (write only).
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   // bind with vertex buffer
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPU can write 
    vbDesc.MiscFlags = 0;
    vbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vrData;
    vrData.pSysMem = vertices;
    vrData.SysMemPitch = 0;
    vrData.SysMemSlicePitch = 0;


    hr = device->CreateBuffer(&vbDesc, &vrData, m_pVertexBuffer.GetAddressOf());
    if (FAILED(hr)) return;


    // Create RasterizerState
    D3D11_RASTERIZER_DESC rrDesc = {};
    rrDesc.AntialiasedLineEnable = false;
    rrDesc.CullMode = D3D11_CULL_NONE;
    rrDesc.DepthBias = 0;
    rrDesc.DepthBiasClamp = 0.0f;
    rrDesc.DepthClipEnable = FALSE;
    rrDesc.FillMode = D3D11_FILL_SOLID;
    rrDesc.FrontCounterClockwise = FALSE;
    rrDesc.MultisampleEnable = FALSE;
    rrDesc.ScissorEnable = FALSE;
    rrDesc.SlopeScaledDepthBias = 0.0f;
    device->CreateRasterizerState(&rrDesc, m_pRasterizerState.GetAddressOf());



}


Sprite::Sprite(D3D::DevicePtr& device, const wchar_t* filename)
{
    HRESULT hr = S_OK;

    m_pTexture = std::make_unique<Texture>();
    m_pTexture->Load(device, filename);

    // 頂点情報のセット
    vertex vertices[] = {
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
    };


    // 頂点バッファ―オブジェクトの生成
    D3D11_BUFFER_DESC vbDesc;
    vbDesc.ByteWidth = sizeof(vertex) * 4;
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;        // accessable by GPU(read only) and CPU (write only).
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   // bind with vertex buffer
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPU can write 
    vbDesc.MiscFlags = 0;
    vbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vrData;
    vrData.pSysMem = vertices;
    vrData.SysMemPitch = 0;
    vrData.SysMemSlicePitch = 0;


    hr = device->CreateBuffer(&vbDesc, &vrData, m_pVertexBuffer.GetAddressOf());
    if (FAILED(hr)) return;


    // Create RasterizerState
    D3D11_RASTERIZER_DESC rrDesc = {};
    rrDesc.AntialiasedLineEnable = false;
    rrDesc.CullMode = D3D11_CULL_NONE;
    rrDesc.DepthBias = 0;
    rrDesc.DepthBiasClamp = 0.0f;
    rrDesc.DepthClipEnable = FALSE;
    rrDesc.FillMode = D3D11_FILL_SOLID;
    rrDesc.FrontCounterClockwise = FALSE;
    rrDesc.MultisampleEnable = FALSE;
    rrDesc.ScissorEnable = FALSE;
    rrDesc.SlopeScaledDepthBias = 0.0f;
    device->CreateRasterizerState(&rrDesc, m_pRasterizerState.GetAddressOf());



}

Sprite::~Sprite()
{
}

// @brief RenderImGui sprite
// @param pos    : Coordinate of lefttop corner in screen space
// @param size   : width, height of sprite
// @param angle  : Rotation angle (degree)
// @param color  : Color of vertices
void Sprite::Render(
    D3D::DeviceContextPtr& imm_context,
    std::shared_ptr<Shader>& p_shader,
    const Vector2& pos,
    const Vector2& size,
    const Vector2& tex_pos,
    const Vector2& tex_size,
    const float angle,
    const Vector4& color)
{

    p_shader->activateShaders(imm_context);

    UINT stride = sizeof(vertex);
    UINT offset = 0;
    D3D11_VIEWPORT viewport;
    UINT numVp = 1;
    imm_context->RSGetViewports(&numVp, &viewport);

    vertex vertices[4] = {
        { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), color },
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), color },
        { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), color },
        { XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), color },
    };

    float radian = angle * 3.14f / 180.0f;

    float sinValue = sinf(radian);
    float cosValue = cosf(radian);
    float mx = (tex_size.x * size.x * 0.5f) / tex_size.x;
    float my = (tex_size.y * size.y * 0.5f) / tex_size.y;
    Vector2 textureSize = m_pTexture->GetTexSize();

    for (auto i = 0; i < 4; ++i)
    {
        vertices[i].position.x *= size.x;
        vertices[i].position.y *= size.y;


        vertices[i].position.x -= mx;
        vertices[i].position.y -= my;

        float rx = vertices[i].position.x;
        float ry = vertices[i].position.y;
        vertices[i].position.x = rx * cosValue - ry * sinValue;
        vertices[i].position.y = rx * sinValue + ry * cosValue;

        vertices[i].position.x += mx;
        vertices[i].position.y += my;

        vertices[i].position.x += (pos.x - 0.5f * size.x);
        vertices[i].position.y += (pos.y - 0.5f * size.y);

        vertices[i].position.x = -1.0f + vertices[i].position.x * 2.0f / viewport.Width;
        vertices[i].position.y = 1.0f - vertices[i].position.y * 2.0f / viewport.Height;

        vertices[i].texcoord.x = (tex_pos.x + vertices[i].texcoord.x * tex_size.x) / static_cast<UINT>(textureSize.x);
        vertices[i].texcoord.y = (tex_pos.y + vertices[i].texcoord.y * tex_size.y) / static_cast<UINT>(textureSize.y);
    }

    D3D11_MAPPED_SUBRESOURCE mrData = {};
    //mrData.pData;
    //mrData.RowPitch;
    //mrData.DepthPitch;

    // Update VertexBuffer from calcuration result
    imm_context->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &mrData);
    memcpy(mrData.pData, vertices, sizeof(vertices));
    imm_context->Unmap(m_pVertexBuffer.Get(), 0);




    imm_context->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

    imm_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


    imm_context->RSSetState(m_pRasterizerState.Get());

    // Set Texture
    m_pTexture->Set(imm_context);

    // �Eプリミティブの描画
    imm_context->Draw(4, 0);
}


void Sprite::Render(
    D3D::DeviceContextPtr& imm_context,
    std::shared_ptr<Shader>& p_shader,
    std::unique_ptr<Texture>& p_texture,
    const Vector2& pos,
    const Vector2& size,
    const Vector2& tex_pos, 
    const Vector2& tex_size,
    const float angle, 
    const Vector4& color)
{
    p_shader->activateShaders(imm_context);

    UINT stride = sizeof(vertex);
    UINT offset = 0;
    D3D11_VIEWPORT viewport;
    UINT numVp = 1;
    imm_context->RSGetViewports(&numVp, &viewport);

    vertex vertices[4] = {
        { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), color },
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), color },
        { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), color },
        { XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), color },
    };

    float radian = angle * 3.14f / 180.0f;

    float sinValue = sinf(radian);
    float cosValue = cosf(radian);
    float mx = (tex_size.x * size.x * 0.5f) / tex_size.x;
    float my = (tex_size.y * size.y * 0.5f) / tex_size.y;
    Vector2 textureSize = p_texture->GetTexSize();

    for (auto i = 0; i < 4; ++i)
    {
        vertices[i].position.x *= size.x;
        vertices[i].position.y *= size.y;


        vertices[i].position.x -= mx;
        vertices[i].position.y -= my;

        float rx = vertices[i].position.x;
        float ry = vertices[i].position.y;
        vertices[i].position.x = rx * cosValue - ry * sinValue;
        vertices[i].position.y = rx * sinValue + ry * cosValue;

        vertices[i].position.x += mx;
        vertices[i].position.y += my;

        vertices[i].position.x += (pos.x - 0.5f * size.x);
        vertices[i].position.y += (pos.y - 0.5f * size.y);

        vertices[i].position.x = -1.0f + vertices[i].position.x * 2.0f / viewport.Width;
        vertices[i].position.y = 1.0f - vertices[i].position.y * 2.0f / viewport.Height;

        vertices[i].texcoord.x = (tex_pos.x + vertices[i].texcoord.x * tex_size.x) / static_cast<UINT>(textureSize.x);
        vertices[i].texcoord.y = (tex_pos.y + vertices[i].texcoord.y * tex_size.y) / static_cast<UINT>(textureSize.y);
    }

    D3D11_MAPPED_SUBRESOURCE mrData = {};
    //mrData.pData;
    //mrData.RowPitch;
    //mrData.DepthPitch;

    // Update VertexBuffer from calcuration result
    imm_context->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &mrData);
    memcpy(mrData.pData, vertices, sizeof(vertices));
    imm_context->Unmap(m_pVertexBuffer.Get(), 0);




    imm_context->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

    imm_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


    imm_context->RSSetState(m_pRasterizerState.Get());

    // Set Texture
    if (p_texture) p_texture->Set(imm_context);

    // �Eプリミティブの描画
    imm_context->Draw(4, 0);

}

void Sprite::TextOutput(
    D3D::DeviceContextPtr& imm_context,
    const std::string& str,
    std::shared_ptr<Shader>& p_shader,
    const Vector2& pos,
    const Vector2& size,
    const Vector4& color)
{
    float sw = 16.0f;
    float sh = 16.0f;
    float cursor = 0.0f;
    int row = 0, line = 0;
    for (const auto& c : str)
    {
        Vector2 pos(pos.x + cursor, pos.y);
        Vector2 texpos(sw * (c & 0x0F), sh * (c >> 4));
        Vector2 texSize(sw, sh);

        Render(imm_context,
            p_shader,
            pos,
            size,
            texpos,
            texSize,
            0.0f,
            color
            );
        cursor += size.x;
    }
}
//******************************************************************************



//******************************************************************************
//
//      SpriteBatch
//
//******************************************************************************
SpriteBatch::SpriteBatch(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t* filename, const size_t maxInstances)
    :MAX_INSTANCES(maxInstances)
{
    HRESULT hr = S_OK;

    Vertex vertices[] = {
        {XMFLOAT3(0,0,0), XMFLOAT2(0,0)},
        {XMFLOAT3(1,0,0), XMFLOAT2(1, 0)},
        { XMFLOAT3(0,1,0), XMFLOAT2(0, 1) },
        { XMFLOAT3(1,1,0), XMFLOAT2(1, 1) }
    };

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    vbDesc.MiscFlags = 0;
    vbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA srData = {};
    srData.pSysMem = vertices;
    srData.SysMemPitch = 0;
    srData.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&vbDesc, &srData, mVertexBuffer.GetAddressOf());
    if (FAILED(hr)) return;



    D3D11_INPUT_ELEMENT_DESC layouts[] = {
        { "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,      0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,         0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };

    ResourceManager::CreateVSFromFile(device, "./Data/Shaders/sprite_batch_vs.cso", mVertexShader.GetAddressOf(), mInputLayout.GetAddressOf(), layouts, ARRAYSIZE(layouts));
    ResourceManager::CreatePSFromFile(device, "./Data/Shaders/sprite_batch_ps.cso", mPixelShader.GetAddressOf());

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




    D3D11_RASTERIZER_DESC rrDesc = {};
    rrDesc.AntialiasedLineEnable = FALSE;
    rrDesc.FillMode = D3D11_FILL_SOLID;
    rrDesc.CullMode = D3D11_CULL_NONE;
    rrDesc.FrontCounterClockwise = FALSE;
    rrDesc.DepthBias = 0;
    rrDesc.DepthBiasClamp = 0;
    rrDesc.SlopeScaledDepthBias = 0;
    rrDesc.DepthClipEnable = FALSE;
    rrDesc.ScissorEnable = FALSE;
    rrDesc.MultisampleEnable = FALSE;
    hr = device->CreateRasterizerState(&rrDesc, mRasterizerState.GetAddressOf());
    if (FAILED(hr)) return;

    hr = ResourceManager::LoadTexFile(device, filename, mSRView.GetAddressOf(), &mTexDesc);
    if (FAILED(hr)) return;

    FLOAT borderColor[4] = { 0, 0, 0, 0 };
    D3D11_SAMPLER_DESC sDesc = {};
    sDesc.Filter = D3D11_FILTER_ANISOTROPIC; //UNIT.06
    sDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sDesc.MipLODBias = 0;
    sDesc.MaxAnisotropy = 16;
    sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    memcpy(sDesc.BorderColor, borderColor, sizeof(FLOAT) * ARRAYSIZE(borderColor));
    sDesc.MinLOD = 0;
    sDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = device->CreateSamplerState(&sDesc, mSamplerState.GetAddressOf());
    if (FAILED(hr)) return;

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
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
    hr = device->CreateDepthStencilState(&dsDesc, mDepthStencilState.GetAddressOf());
    if (FAILED(hr)) return;

}

SpriteBatch::~SpriteBatch()
{

}

void SpriteBatch::Begin(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context)
{
    HRESULT hr = S_OK;
    UINT strides[2] = { sizeof(Vertex), sizeof(Instance) };
    UINT offsets[2] = { 0,0 };

    ID3D11Buffer* vbs[2] = { mVertexBuffer.Get(), mInstanceBuffer.Get() };
    imm_context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
    imm_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    imm_context->IASetInputLayout(mInputLayout.Get());

    imm_context->OMSetDepthStencilState(mDepthStencilState.Get(), 1);
    imm_context->RSSetState(mRasterizerState.Get());
    imm_context->VSSetShader(mVertexShader.Get(), nullptr, 0);
    imm_context->PSSetShader(mPixelShader.Get(), nullptr, 0);
    imm_context->PSSetShaderResources(0, 1, mSRView.GetAddressOf());
    imm_context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

    UINT numViewport = 1;
    imm_context->RSGetViewports(&numViewport, &mViewport);

    D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
    D3D11_MAPPED_SUBRESOURCE mappedBuffer;
    hr = imm_context->Map(mInstanceBuffer.Get(), 0, map, 0, &mappedBuffer);
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    mInstance = static_cast<Instance*>(mappedBuffer.pData);

    mCountInstances = 0;
}

void SpriteBatch::Render(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context, 
    const DirectX::XMFLOAT2& pos, 
    const DirectX::XMFLOAT2& size, 
    const DirectX::XMFLOAT2 &texPos, 
    const DirectX::XMFLOAT2 &texSize,
    const float angle, 
    const DirectX::XMFLOAT4& color)
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
        );
    XMStoreFloat4x4(&mInstance[mCountInstances].ndc_transform, DirectX::XMMatrixTranspose(M * N));

    float tw = static_cast<float>(mTexDesc.Width);
    float th = static_cast<float>(mTexDesc.Height);
    mInstance[mCountInstances].texcoord_transform = DirectX::XMFLOAT4(texPos.x / tw, texPos.y / th, texSize.x / tw, texSize.y / th);
    mInstance[mCountInstances].color = color;


    mCountInstances++;
}

void SpriteBatch::RenderText(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context, 
    const std::string& text,
    const DirectX::XMFLOAT2& pos,
    const DirectX::XMFLOAT2& size,
    const DirectX::XMFLOAT4& color)
{
    float sw = 16.0f;
    float sh = 16.0f;
    float cursor = 0.0f;
    int row = 0, line = 0;
    
    for (auto& c : text)
    {
        XMFLOAT2 pos(pos.x + cursor, pos.y);
        XMFLOAT2 texPos(sw * (c & 0x0F), sh * (c >> 4));
        XMFLOAT2 texSize(sw, sh);

        Render(imm_context,
            pos,
            size,
            texPos,
            texSize,
            0.0f,
            color
        );
        cursor += size.x;
    }
}

void SpriteBatch::End(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context)
{
    imm_context->Unmap(mInstanceBuffer.Get(), 0);
    imm_context->DrawInstanced(4, mCountInstances, 0, 0);
}