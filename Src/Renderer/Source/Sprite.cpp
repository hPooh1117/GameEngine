#include "Sprite.h"

#include "ResourceManager.h"
#include "./Utilities/misc.h"

//---< namespace >-----------------------------------------------------------------------------------------------------
using namespace DirectX;


//******************************************************************************
//
//      sprite
//
//******************************************************************************
Sprite::Sprite(Graphics::GraphicsDevice* p_device):
    mpVertexBuffer(std::make_unique<Graphics::GPUBuffer>())
{
    HRESULT hr = S_OK;
    auto& pDevice = p_device->GetDevicePtr();
    mpTexture = std::unique_ptr<Texture>();

    // 頂点情報のセット
    vertex vertices[] = {
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
    };


    //// 頂点バッファ―オブジェクトの生成
    //D3D11_BUFFER_DESC vbDesc;
    //vbDesc.ByteWidth = sizeof(vertex) * 4;
    //vbDesc.Usage = D3D11_USAGE_DYNAMIC;        // accessable by GPU(read only) and CPU (write only).
    //vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   // bind with vertex buffer
    //vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPU can write 
    //vbDesc.MiscFlags = 0;
    //vbDesc.StructureByteStride = 0;

    //D3D11_SUBRESOURCE_DATA vrData;
    //vrData.pSysMem = vertices;
    //vrData.SysMemPitch = 0;
    //vrData.SysMemSlicePitch = 0;


    //hr = pDevice->CreateBuffer(&vbDesc, &vrData, m_pVertexBuffer.GetAddressOf());
    //if (FAILED(hr)) return;
    Graphics::GPUBufferDesc& desc = mpVertexBuffer->desc;
    desc.ByteWidth = sizeof(vertex) * 4;
    desc.BindFlags = Graphics::BIND_VERTEX_BUFFER;
    desc.Usage = Graphics::USAGE_DYNAMIC;
    desc.CPUAccessFlags = Graphics::CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    Graphics::SubresourceData data = {};
    data.pSysMem = vertices;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    p_device->CreateBuffer(&desc, &data, mpVertexBuffer.get());

    //// Create RasterizerState
    //D3D11_RASTERIZER_DESC rrDesc = {};
    //rrDesc.AntialiasedLineEnable = false;
    //rrDesc.CullMode = D3D11_CULL_NONE;
    //rrDesc.DepthBias = 0;
    //rrDesc.DepthBiasClamp = 0.0f;
    //rrDesc.DepthClipEnable = FALSE;
    //rrDesc.FillMode = D3D11_FILL_SOLID;
    //rrDesc.FrontCounterClockwise = FALSE;
    //rrDesc.MultisampleEnable = FALSE;
    //rrDesc.ScissorEnable = FALSE;
    //rrDesc.SlopeScaledDepthBias = 0.0f;
    //pDevice->CreateRasterizerState(&rrDesc, m_pRasterizerState.GetAddressOf());



}


Sprite::Sprite(Graphics::GraphicsDevice* p_device, const wchar_t* filename):
    mpVertexBuffer(std::make_unique<Graphics::GPUBuffer>())
{
    HRESULT hr = S_OK;
    auto& pDevice = p_device->GetDevicePtr();
    mpTexture = std::make_unique<Texture>();
    mpTexture->Load(pDevice, filename);

    // 頂点情報のセット
    vertex vertices[] = {
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
        { XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0), XMFLOAT4(0, 0, 0, 0) },
    };


    //// 頂点バッファ―オブジェクトの生成
    //D3D11_BUFFER_DESC vbDesc;
    //vbDesc.ByteWidth = sizeof(vertex) * 4;
    //vbDesc.Usage = D3D11_USAGE_DYNAMIC;        // accessable by GPU(read only) and CPU (write only).
    //vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   // bind with vertex buffer
    //vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPU can write 
    //vbDesc.MiscFlags = 0;
    //vbDesc.StructureByteStride = 0;

    //D3D11_SUBRESOURCE_DATA vrData;
    //vrData.pSysMem = vertices;
    //vrData.SysMemPitch = 0;
    //vrData.SysMemSlicePitch = 0;


    //hr = pDevice->CreateBuffer(&vbDesc, &vrData, m_pVertexBuffer.GetAddressOf());
    //if (FAILED(hr)) return;

    Graphics::GPUBufferDesc& desc = mpVertexBuffer->desc;
    desc.ByteWidth = sizeof(vertex) * 4;
    desc.BindFlags = Graphics::BIND_VERTEX_BUFFER;
    desc.Usage = Graphics::USAGE_DYNAMIC;
    desc.CPUAccessFlags = Graphics::CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    Graphics::SubresourceData data = {};
    data.pSysMem = vertices;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    p_device->CreateBuffer(&desc, &data, mpVertexBuffer.get());


    //// Create RasterizerState
    //D3D11_RASTERIZER_DESC rrDesc = {};
    //rrDesc.AntialiasedLineEnable = false;
    //rrDesc.CullMode = D3D11_CULL_NONE;
    //rrDesc.DepthBias = 0;
    //rrDesc.DepthBiasClamp = 0.0f;
    //rrDesc.DepthClipEnable = FALSE;
    //rrDesc.FillMode = D3D11_FILL_SOLID;
    //rrDesc.FrontCounterClockwise = FALSE;
    //rrDesc.MultisampleEnable = FALSE;
    //rrDesc.ScissorEnable = FALSE;
    //rrDesc.SlopeScaledDepthBias = 0.0f;
    //pDevice->CreateRasterizerState(&rrDesc, m_pRasterizerState.GetAddressOf());



}

Sprite::~Sprite()
{
}

// @brief RenderUIQueue sprite
// @param pos    : Coordinate of lefttop corner in screen space
// @param size   : width, height of sprite
// @param angle  : Rotation angle (degree)
// @param color  : Color of vertices
void Sprite::Render(
    Graphics::GraphicsDevice* p_device,
    Shader* p_shader,
    const Vector2& pos,
    const Vector2& size,
    const Vector2& tex_pos,
    const Vector2& tex_size,
    const float angle,
    const Vector4& color)
{
    auto& ImmContext = p_device->GetImmContextPtr();
    if (p_shader != nullptr) p_shader->ActivateShaders(ImmContext);

    UINT stride = sizeof(vertex);
    UINT offset = 0;
    D3D11_VIEWPORT viewport;
    UINT numVp = 1;
    ImmContext->RSGetViewports(&numVp, &viewport);

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
    Vector2 textureSize = mpTexture->GetTexSize();

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


    p_device->UpdateBuffer(mpVertexBuffer.get(), vertices, sizeof(vertices));

    Graphics::GPUBuffer* vb = mpVertexBuffer.get();
    p_device->BindVertexBuffer(&vb, 0, 1, &stride, &offset);
    p_device->BindPrimitiveTopology(Graphics::TRIANGLESTRIP);
    p_device->RSSetState(Graphics::RS_SPRITE);
    p_device->OMSetDepthStencilState(Graphics::DS_FALSE);

    // Set Texture
    mpTexture->Set(ImmContext);

    p_device->Draw(4, 0);
}


void Sprite::Render(
    Graphics::GraphicsDevice* p_device,
    Shader* p_shader,
    std::unique_ptr<Texture>& p_texture,
    const Vector2& pos,
    const Vector2& size,
    const Vector2& tex_pos, 
    const Vector2& tex_size,
    const float angle, 
    const Vector4& color)
{
    auto& ImmContext = p_device->GetImmContextPtr();
    if (p_shader != nullptr) p_shader->ActivateShaders(ImmContext);

    UINT stride = sizeof(vertex);
    UINT offset = 0;
    D3D11_VIEWPORT viewport;
    UINT numVp = 1;
    ImmContext->RSGetViewports(&numVp, &viewport);

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

    p_device->UpdateBuffer(mpVertexBuffer.get(), vertices, sizeof(vertices));


    Graphics::GPUBuffer* vb = mpVertexBuffer.get();
    p_device->BindVertexBuffer(&vb, 0, 1, &stride, &offset);
    p_device->BindPrimitiveTopology(Graphics::TRIANGLESTRIP);
    p_device->RSSetState(Graphics::RS_SPRITE);
    p_device->OMSetDepthStencilState(Graphics::DS_FALSE);

    // Set Texture
    if (p_texture) p_texture->Set(ImmContext);

    p_device->Draw(4, 0);

}

void Sprite::RenderScreen(Graphics::GraphicsDevice* p_device,
    Shader* p_shader, const Vector2& pos, const Vector2& size)
{
    auto& ImmContext = p_device->GetImmContextPtr();
    if (p_shader != nullptr) p_shader->ActivateShaders(ImmContext);

    UINT stride = sizeof(vertex);
    UINT offset = 0;
    D3D11_VIEWPORT viewport;
    UINT numVp = 1;
    ImmContext->RSGetViewports(&numVp, &viewport);

    vertex vertices[4] = {
        { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT4(1, 1, 1, 1) },
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT4(1, 1, 1, 1) },
        { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1, 1, 1, 1) },
        { XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT4(1, 1, 1, 1) },
    };

    float radian = 0.0f;
    Vector2 texSize = { static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT) };

    float sinValue = sinf(radian);
    float cosValue = cosf(radian);
    float mx = (texSize.x * size.x * 0.5f) / texSize.x;
    float my = (texSize.y * size.y * 0.5f) / texSize.y;

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

        vertices[i].texcoord.x = (vertices[i].texcoord.x * texSize.x) / static_cast<UINT>(texSize.x);
        vertices[i].texcoord.y = (vertices[i].texcoord.y * texSize.y) / static_cast<UINT>(texSize.y);
    }

    p_device->UpdateBuffer(mpVertexBuffer.get(), vertices, sizeof(vertices));


    Graphics::GPUBuffer* vb = mpVertexBuffer.get();
    p_device->BindVertexBuffer(&vb, 0, 1, &stride, &offset);
    p_device->BindPrimitiveTopology(Graphics::TRIANGLESTRIP);
    p_device->RSSetState(Graphics::RS_SPRITE);
    p_device->OMSetDepthStencilState(Graphics::DS_FALSE);

    p_device->Draw(4, 0);
}

void Sprite::TextOutput(
    Graphics::GraphicsDevice* p_device,
    const std::string& str,
    Shader* p_shader,
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

        Render(p_device,
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
SpriteBatch::SpriteBatch(Graphics::GraphicsDevice* p_device, const wchar_t* filename, const size_t maxInstances)
    :MAX_INSTANCES(maxInstances)
{
    HRESULT hr = S_OK;
    auto& pDevice = p_device->GetDevicePtr();
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

    hr = pDevice->CreateBuffer(&vbDesc, &srData, mpVertexBuffer.GetAddressOf());
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

    ResourceManager::CreateVSFromFile(pDevice, "./Data/Shaders/sprite_batch_vs.cso", mVertexShader.GetAddressOf(), mInputLayout.GetAddressOf(), layouts, ARRAYSIZE(layouts));
    ResourceManager::CreatePSFromFile(pDevice, "./Data/Shaders/sprite_batch_ps.cso", mPixelShader.GetAddressOf());

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

        hr = pDevice->CreateBuffer(&ibDesc, &srData, mInstanceBuffer.GetAddressOf());
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
    hr = pDevice->CreateRasterizerState(&rrDesc, mRasterizerState.GetAddressOf());
    if (FAILED(hr)) return;

    hr = ResourceManager::LoadTexFile(pDevice, filename, mSRView.GetAddressOf(), &mTexDesc);
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
    hr = pDevice->CreateSamplerState(&sDesc, mSamplerState.GetAddressOf());
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
    hr = pDevice->CreateDepthStencilState(&dsDesc, mDepthStencilState.GetAddressOf());
    if (FAILED(hr)) return;

}

SpriteBatch::~SpriteBatch()
{

}

void SpriteBatch::Begin(Graphics::GraphicsDevice* p_device)
{
    HRESULT hr = S_OK;
    auto& ImmContext = p_device->GetImmContextPtr();
    UINT strides[2] = { sizeof(Vertex), sizeof(Instance) };
    UINT offsets[2] = { 0,0 };

    ID3D11Buffer* vbs[2] = { mpVertexBuffer.Get(), mInstanceBuffer.Get() };
    ImmContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
    ImmContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    ImmContext->IASetInputLayout(mInputLayout.Get());

    ImmContext->OMSetDepthStencilState(mDepthStencilState.Get(), 1);
    ImmContext->RSSetState(mRasterizerState.Get());
    ImmContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
    ImmContext->PSSetShader(mPixelShader.Get(), nullptr, 0);
    ImmContext->PSSetShaderResources(0, 1, mSRView.GetAddressOf());
    ImmContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

    UINT numViewport = 1;
    ImmContext->RSGetViewports(&numViewport, &mViewport);

    D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
    D3D11_MAPPED_SUBRESOURCE mappedBuffer;
    hr = ImmContext->Map(mInstanceBuffer.Get(), 0, map, 0, &mappedBuffer);
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    mInstance = static_cast<Instance*>(mappedBuffer.pData);

    mCountInstances = 0;
}

void SpriteBatch::Render(
    Graphics::GraphicsDevice* p_device,
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
    Graphics::GraphicsDevice* p_device,
    const std::string& text,
    const DirectX::XMFLOAT2& pos,
    const DirectX::XMFLOAT2& size,
    const DirectX::XMFLOAT4& color)
{
    auto& ImmContext = p_device->GetImmContextPtr();

    float sw = 16.0f;
    float sh = 16.0f;
    float cursor = 0.0f;
    int row = 0, line = 0;
    
    for (auto& c : text)
    {
        XMFLOAT2 pos(pos.x + cursor, pos.y);
        XMFLOAT2 texPos(sw * (c & 0x0F), sh * (c >> 4));
        XMFLOAT2 texSize(sw, sh);

        Render(p_device,
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

void SpriteBatch::End(Graphics::GraphicsDevice* p_device)
{
    auto& ImmContext = p_device->GetImmContextPtr();
    ImmContext->Unmap(mInstanceBuffer.Get(), 0);
    ImmContext->DrawInstanced(4, mCountInstances, 0, 0);
}