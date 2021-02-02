#include "GeometricPrimitiveSelf.h"

#include <vector>

#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"

#include "./Engine/CameraController.h"

#include "./Utilities/misc.h"

using namespace DirectX;


//******************************************************************************
//
//      GeometricPrimitiveSelf
//
//******************************************************************************
GeometricPrimitiveSelf::GeometricPrimitiveSelf(
    Graphics::GraphicsDevice* p_device,
    D3D11_PRIMITIVE_TOPOLOGY topology) 
    : Mesh(),
    mpVertexBuffer(new Graphics::GPUBuffer()),
    mpIndexBuffer(new Graphics::GPUBuffer())
{
    HRESULT hr = S_OK;
    auto& pDevice = p_device->GetDevicePtr();

    //mTopologyType = topology;
    

    //Graphics::GPUBufferDesc desc = {};
    //desc.ByteWidth = sizeof(CBufferForMesh);
    //desc.BindFlags = Graphics::BindFlagID::BIND_CONSTANT_BUFFER;
    //desc.Usage = Graphics::UsageID::USAGE_DEFAULT;
    //desc.CPUAccessFlags = 0;
    //desc.MiscFlags = 0;
    //desc.StructureByteStride = 0;

    //p_device->CreateBuffer(&desc, nullptr, )

    //--> Create Constant Buffer
    hr = pDevice->CreateBuffer(
        &CD3D11_BUFFER_DESC(
            sizeof(CBufferForMesh),
            D3D11_BIND_CONSTANT_BUFFER,
            D3D11_USAGE_DEFAULT,
            0,
            0,
            0
            ),
        nullptr,
        mpConstantBufferMesh.GetAddressOf()
        );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    hr = pDevice->CreateBuffer(
        &CD3D11_BUFFER_DESC(
            sizeof(CBufferForMaterial),
            D3D11_BIND_CONSTANT_BUFFER,
            D3D11_USAGE_DEFAULT,
            0,
            0,
            0
        ),
        nullptr,
        mpConstantBufferMaterial.GetAddressOf()
    );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));




    ////-->Create RasterizerState
    //hr = pDevice->CreateRasterizerState(
    //    &CD3D11_RASTERIZER_DESC(
    //        D3D11_FILL_WIREFRAME,     // D3D11_FILL_MODE FillMode;
    //        D3D11_CULL_BACK,          // D3D11_CULL_MODE CullMode;
    //        FALSE,                    // BOOL FrontCounterClockwise;
    //        0,                        // INT DepthBias;
    //        0.0f,                     // FLOAT DepthBiasClamp;
    //        0.0f,                     // FLOAT SlopeScaledDepthBias;
    //        TRUE,                     // BOOL DepthClipEnable;
    //        FALSE,                    // BOOL ScissorEnable;
    //        FALSE,                    // BOOL MultisampleEnable;
    //        FALSE                      // BOOL AntialiasedLineEnable;
    //        ),
    //    m_pRasterizerWire.GetAddressOf()
    //    );
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    //hr = pDevice->CreateRasterizerState(
    //    &CD3D11_RASTERIZER_DESC(
    //        D3D11_FILL_SOLID,         // D3D11_FILL_MODE FillMode;
    //        D3D11_CULL_BACK,          // D3D11_CULL_MODE CullMode;
    //        FALSE,                    // BOOL FrontCounterClockwise;
    //        0,                        // INT DepthBias;
    //        0.0f,                     // FLOAT DepthBiasClamp;
    //        0.0f,                     // FLOAT SlopeScaledDepthBias;
    //        TRUE,                    // BOOL DepthClipEnable;
    //        FALSE,                    // BOOL ScissorEnable;
    //        FALSE,                    // BOOL MultisampleEnable;
    //        FALSE                      // BOOL AntialiasedLineEnable;
    //        ),
    //    m_pRasterizerSolid.GetAddressOf()
    //    );
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));


}

GeometricPrimitiveSelf::~GeometricPrimitiveSelf()
{
}



void GeometricPrimitiveSelf::CreateBuffers(Graphics::GraphicsDevice* p_device)
{
    HRESULT hr = S_OK;
    auto pDevice = p_device->GetDevicePtr();
    //--> Create Vertex Buffer
    //D3D11_SUBRESOURCE_DATA vSrData = {};
    //vSrData.pSysMem = mVertices.data();
    //vSrData.SysMemPitch = 0;
    //vSrData.SysMemSlicePitch = 0;

    ////hr = pDev->CreateBuffer(&vertBufDesc, &vSrData, mpVertexBuffer.GetAddressOf());
    ////_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
    //hr = pDevice->CreateBuffer(
    //    &CD3D11_BUFFER_DESC(
    //        sizeof(Vertex) * mVertices.size(),
    //        D3D11_BIND_VERTEX_BUFFER,
    //        D3D11_USAGE_IMMUTABLE,
    //        0,
    //        0,
    //        0
    //    ),
    //    &vSrData,
    //    m_pVertexBuffer.GetAddressOf()
    //);
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    //


    //D3D11_SUBRESOURCE_DATA iSrData = {};
    //iSrData.pSysMem = mIndices.data();
    //iSrData.SysMemPitch = 0;
    //iSrData.SysMemSlicePitch = 0;

    ////hr = pDev->CreateBuffer(&idxBuffDesc, &iSrData, mpIndexBuffer.GetAddressOf());
    ////_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    //hr = pDevice->CreateBuffer(
    //    &CD3D11_BUFFER_DESC(
    //        sizeof(u_int) * mIndices.size(),
    //        D3D11_BIND_INDEX_BUFFER,
    //        D3D11_USAGE_IMMUTABLE,
    //        0,
    //        0,
    //        0
    //    ),
    //    &iSrData,
    //    m_pIndexBuffer.GetAddressOf()
    //);
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    Graphics::GPUBufferDesc desc = {};
    desc.ByteWidth = sizeof(Vertex) * mVertices.size();
    desc.BindFlags = Graphics::BIND_VERTEX_BUFFER;
    desc.Usage = Graphics::USAGE_IMMUTABLE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    Graphics::SubresourceData data = {};
    data.pSysMem = mVertices.data();
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    p_device->CreateBuffer(&desc, &data, mpVertexBuffer.get());

    desc.ByteWidth = sizeof(uint32_t) * mIndices.size();
    desc.BindFlags = Graphics::BIND_INDEX_BUFFER;
    data.pSysMem = mIndices.data();

    p_device->CreateBuffer(&desc, &data, mpIndexBuffer.get());
    
}



void GeometricPrimitiveSelf::Render(
    Graphics::GraphicsDevice* p_device,
    float elapsed_time,
    const DirectX::XMMATRIX& world,
    CameraController* camera,
    Shader* shader,
    const Material& mat_data,
    bool isShadow,
    bool isSolid
)
{
    HRESULT hr = S_OK;
    auto& ImmContext = p_device->GetImmContextPtr();
    

    if (shader != nullptr) shader->ActivateShaders(ImmContext);

    isSolid ? p_device->RSSetState(Graphics::RS_BACK)
        : p_device->RSSetState(Graphics::RS_WIRE_BACK);

    SetRenderState(p_device);


    
    DirectX::XMStoreFloat4x4(&mMeshData.world, world);
    DirectX::XMStoreFloat4x4(&mMeshData.WVP, world * (isShadow ? camera->GetOrthoView() * camera->GetOrthoProj(ImmContext) : camera->GetViewMatrix() * camera->GetProjMatrix(ImmContext)));
    DirectX::XMStoreFloat4x4(&mMeshData.invViewProj, camera->GetInvProjViewMatrix(ImmContext));
    DirectX::XMStoreFloat4x4(&mMeshData.invView, camera->GetInvViewMatrix());
    DirectX::XMStoreFloat4x4(&mMeshData.invProj, camera->GetInvProjMatrix(ImmContext));

    CBufferForMaterial matData = {};
    matData.mat_color = mat_data.mat_color;
    matData.metalness = mat_data.metalness;
    matData.roughness = mat_data.roughness;
    matData.specularColor = mat_data.specularColor;
    matData.textureConfig = mat_data.textureConfig;
    matData.diffuse = mat_data.diffuse;
    matData.specular = mat_data.specular;

    ImmContext->UpdateSubresource(mpConstantBufferMesh.Get(), 0, nullptr, &mMeshData, 0, 0);
    ImmContext->VSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
    ImmContext->HSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
    ImmContext->DSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
    ImmContext->GSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
    ImmContext->PSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());

    ImmContext->UpdateSubresource(mpConstantBufferMaterial.Get(), 0, nullptr, &matData, 0, 0);
    ImmContext->VSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
    ImmContext->HSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
    ImmContext->DSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
    ImmContext->GSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
    ImmContext->PSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());


    uint32_t stride = sizeof(Vertex);
    uint32_t offset = 0;

    Graphics::GPUBuffer* vb = mpVertexBuffer.get();
    p_device->BindVertexBuffer(&vb, 0, 1, &stride, &offset);
    p_device->BindIndexBuffer(mpIndexBuffer.get(), Graphics::IndexBufferFormat::INDEXFORMAT_32BIT, 0);
    p_device->BindPrimitiveTopology(mToplogyID);


    
    p_device->DrawIndexed(mpIndexBuffer->desc.ByteWidth / sizeof(uint32_t), 0, 0);
}

//******************************************************************************
//
//      BasicLine
//
//******************************************************************************
BasicLine::BasicLine(Graphics::GraphicsDevice* p_device)
    :GeometricPrimitiveSelf(p_device, D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
{
    auto& pDevice = p_device->GetDevicePtr();

    Vertex vertices[] =
    {
        { XMFLOAT3(-0.5, +0.0, +0.0), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1)},
        { XMFLOAT3(+0.5, +0.0, +0.0), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1)},
    };
    uint32_t indices[] =
    {
        0 , 1
    };

    for (auto& vertex : vertices)
    {
        mVertices.emplace_back(vertex);
    }
    for (auto& index : indices)
    {
        mIndices.emplace_back(index);
    }


    CreateBuffers(p_device/*, vertices, indices, ARRAYSIZE(vertices), ARRAYSIZE(indices)*/);


    //--> Create Constant Buffer
    auto hr = pDevice->CreateBuffer(
        &CD3D11_BUFFER_DESC(
            sizeof(CBufferForMesh),
            D3D11_BIND_CONSTANT_BUFFER,
            D3D11_USAGE_DEFAULT,
            0,
            0,
            0
        ),
        nullptr,
        mConstantBufferForLine.GetAddressOf()
    );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    //if (m_pRasterizerSolid) m_pRasterizerSolid.Reset();
    //hr = pDevice->CreateRasterizerState(
    //    &CD3D11_RASTERIZER_DESC(
    //        D3D11_FILL_SOLID,         // D3D11_FILL_MODE FillMode;
    //        D3D11_CULL_NONE,          // D3D11_CULL_MODE CullMode;
    //        FALSE,                    // BOOL FrontCounterClockwise;
    //        0,                        // INT DepthBias;
    //        0.0f,                     // FLOAT DepthBiasClamp;
    //        0.0f,                     // FLOAT SlopeScaledDepthBias;
    //        TRUE,                    // BOOL DepthClipEnable;
    //        FALSE,                    // BOOL ScissorEnable;
    //        FALSE,                    // BOOL MultisampleEnable;
    //        FALSE                     // BOOL AntialiasedLineEnable;
    //    ),
    //    m_pRasterizerSolid.GetAddressOf()
    //);
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

}

void BasicLine::SetRenderState(Graphics::GraphicsDevice* p_device)
{
    mMeshData.param.x = 4;
    p_device->RSSetState(Graphics::RS_DOUBLESIDED);
}


//******************************************************************************
//
//      BasicCube
//
//******************************************************************************
BasicCube::BasicCube(
    Graphics::GraphicsDevice* p_device)
    :GeometricPrimitiveSelf(p_device)
{
   


    // Set Info of vertices
    Vertex vertices[] = {
        // TOP
        { XMFLOAT3(-0.5, +0.5, +0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1) ,XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +1.0f)},
        { XMFLOAT3(+0.5, +0.5, +0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1) ,XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +1.0f)},
        { XMFLOAT3(-0.5, +0.5, -0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1) ,XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +1.0f)},
        { XMFLOAT3(+0.5, +0.5, -0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1) ,XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +1.0f)},

        // BOTTOM
        { XMFLOAT3(-0.5, -0.5, +0.5), XMFLOAT3(+0.0f, -1.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1) ,XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +1.0f)},
        { XMFLOAT3(+0.5, -0.5, +0.5), XMFLOAT3(+0.0f, -1.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1) ,XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +1.0f)},
        { XMFLOAT3(-0.5, -0.5, -0.5), XMFLOAT3(+0.0f, -1.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1) ,XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +1.0f)},
        { XMFLOAT3(+0.5, -0.5, -0.5), XMFLOAT3(+0.0f, -1.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1) ,XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, +1.0f)},

        // FRONT
        { XMFLOAT3(-0.5, +0.5, -0.5), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},
        { XMFLOAT3(+0.5, +0.5, -0.5), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},
        { XMFLOAT3(-0.5, -0.5, -0.5), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},
        { XMFLOAT3(+0.5, -0.5, -0.5), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},

        // BACK
        { XMFLOAT3(-0.5, +0.5, +0.5), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, -1.0f, +0.0f)},
        { XMFLOAT3(+0.5, +0.5, +0.5), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, -1.0f, +0.0f)},
        { XMFLOAT3(-0.5, -0.5, +0.5), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, -1.0f, +0.0f)},
        { XMFLOAT3(+0.5, -0.5, +0.5), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT3(+0.0f, -1.0f, +0.0f)},

        // RIGHT
        { XMFLOAT3(+0.5, +0.5, -0.5), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},
        { XMFLOAT3(+0.5, +0.5, +0.5), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},
        { XMFLOAT3(+0.5, -0.5, -0.5), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},
        { XMFLOAT3(+0.5, -0.5, +0.5), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},

        // LEFT
        { XMFLOAT3(-0.5, +0.5, -0.5), XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},
        { XMFLOAT3(-0.5, +0.5, +0.5), XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},
        { XMFLOAT3(-0.5, -0.5, -0.5), XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},
        { XMFLOAT3(-0.5, -0.5, +0.5), XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT3(+0.0f, +1.0f, +0.0f)},
    };

    for (auto& vertex : vertices)
    {
        mVertices.emplace_back(vertex);
    }


    // Set Info of Index
    uint32_t indices[] = {
        0, 1, 2,     1,  3, 2,
        4, 6, 5,     5,  6, 7,
        8, 9, 10,    9, 11, 10,
        12, 14, 13, 13, 14, 15,
        16, 17, 18, 17, 19, 18,
        20, 22, 21, 21, 22, 23,
    };


    for (auto& index : indices)
    {
        mIndices.emplace_back(index);
    }

    CreateBuffers(p_device/*, vertices, indices, ARRAYSIZE(vertices), ARRAYSIZE(indices)*/);

}




//******************************************************************************
//
//      BasicCylinder
//
//******************************************************************************
BasicCylinder::BasicCylinder(
    Graphics::GraphicsDevice* p_device,
    unsigned int slices) 
    :GeometricPrimitiveSelf(p_device)
{
    //std::vector<Vertex> vertices;
    //std::vector<WORD> indices;

    float d = 2.0f*DirectX::XM_PI / slices;
    float r = 0.5f;

    Vertex vertex;
    uint32_t base_index = 0;

    // top cap centre
    vertex.position = DirectX::XMFLOAT3(0.0f, +0.5f, 0.0f);
    vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);
    vertex.texcoord = DirectX::XMFLOAT2(0, 0);
    vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
    //vertex.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    //vertex.binormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
    mVertices.emplace_back(vertex);
    // top cap ring
    for (uint32_t i = 0; i < slices; ++i)
    {
        float x = r * cosf(i*d);
        float z = r * sinf(i*d);
        vertex.position = DirectX::XMFLOAT3(x, +0.5f, z);
        vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);
        vertex.texcoord = DirectX::XMFLOAT2(0, 0);
        vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
        //Vector3 tan(-z, 0, x);
        //tan.normalize();
        //vertex.tangent = DirectX::XMFLOAT3(tan.x, 0.0f, tan.z);
        //vertex.binormal = tan.cross(vertex.normal);
        mVertices.emplace_back(vertex);
    }
    base_index = 0;
    for (uint32_t i = 0; i < slices - 1; ++i)
    {
        mIndices.emplace_back(base_index + 0);
        mIndices.emplace_back(base_index + i + 2);
        mIndices.emplace_back(base_index + i + 1);
    }
    mIndices.emplace_back(base_index + 0);
    mIndices.emplace_back(base_index + 1);
    mIndices.emplace_back(base_index + slices);

    // bottom cap centre
    vertex.position = XMFLOAT3(0.0f, -0.5f, 0.0f);
    vertex.normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
    vertex.texcoord = DirectX::XMFLOAT2(0, 0);
    vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
    //vertex.tangent = XMFLOAT3(-1.0f, 0.0f, 0.0f);
    //vertex.binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);

    mVertices.emplace_back(vertex);
    // bottom cap ring
    for (uint32_t i = 0; i < slices; ++i)
    {
        float x = r * cosf(i*d);
        float z = r * sinf(i*d);
        vertex.position = XMFLOAT3(x, -0.5f, z);
        vertex.normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
        vertex.texcoord = DirectX::XMFLOAT2(0, 0);
        vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
        //Vector3 tan(-z, 0, x);
        //tan.normalize();
        //vertex.tangent = DirectX::XMFLOAT3(tan.x, 0.0f, tan.z);
        //vertex.binormal = tan.cross(vertex.normal);

        mVertices.emplace_back(vertex);
    }
    base_index = slices + 1;
    for (uint32_t i = 0; i < slices - 1; ++i)
    {
        mIndices.emplace_back(base_index + 0);
        mIndices.emplace_back(base_index + i + 1);
        mIndices.emplace_back(base_index + i + 2);
    }
    mIndices.emplace_back(base_index + 0);
    mIndices.emplace_back(base_index + (slices - 1) + 1);
    mIndices.emplace_back(base_index + (0) + 1);

    // side rectangle
    for (uint32_t i = 0; i < slices; ++i)
    {
        float x = r * cosf(i*d);
        float z = r * sinf(i*d);

        vertex.position = XMFLOAT3(x, +0.5f, z);
        vertex.normal = XMFLOAT3(x, 0.0f, z);
        vertex.texcoord = DirectX::XMFLOAT2(0, 0);
        vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
        //Vector3 tan(-z, 0, x);
        //tan.normalize();
        //vertex.tangent = DirectX::XMFLOAT3(tan.x, 0.0f, tan.z);
        //vertex.binormal = tan.cross(vertex.normal);

        mVertices.emplace_back(vertex);

        vertex.position = XMFLOAT3(x, -0.5f, z);
        vertex.normal = XMFLOAT3(x, 0.0f, z);
        vertex.texcoord = DirectX::XMFLOAT2(0, 0);
        vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
        //Vector3 tan(-z, 0, x);
        //tan.normalize();
        //vertex.tangent = DirectX::XMFLOAT3(tan.x, 0.0f, tan.z);
        //vertex.binormal = tan.cross(vertex.normal);

        mVertices.emplace_back(vertex);
    }
    base_index = slices * 2 + 2;
    for (uint32_t i = 0; i < slices - 1; ++i)
    {
        mIndices.emplace_back(base_index + i * 2 + 0);
        mIndices.emplace_back(base_index + i * 2 + 2);
        mIndices.emplace_back(base_index + i * 2 + 1);

        mIndices.emplace_back(base_index + i * 2 + 1);
        mIndices.emplace_back(base_index + i * 2 + 2);
        mIndices.emplace_back(base_index + i * 2 + 3);
    }
    mIndices.emplace_back(base_index + (slices - 1) * 2 + 0);
    mIndices.emplace_back(base_index + (0) * 2 + 0);
    mIndices.emplace_back(base_index + (slices - 1) * 2 + 1);

    mIndices.emplace_back(base_index + (slices - 1) * 2 + 1);
    mIndices.emplace_back(base_index + (0) * 2 + 0);
    mIndices.emplace_back(base_index + (0) * 2 + 1);


    CreateBuffers(p_device);


}



//******************************************************************************
//
//      BasicSphere
//
//******************************************************************************
BasicSphere::BasicSphere(
    Graphics::GraphicsDevice* p_device,
    unsigned int slices, 
    unsigned int stacks,
    float radius)
    :GeometricPrimitiveSelf(p_device)
{
    //std::vector<Vertex> vertices;
    //std::vector<WORD> indices;


    //
    // Compute the vertices stating at the top pole and moving down the stacks.
    //

    // Poles: note that there will be texture coordinate distortion as there is
    // not a unique point on the texture map to assign to the pole when mapping
    // a rectangular texture onto a sphere.
    auto GetUV = [&](Vertex& v)
    {
        float phi = atan2f(v.normal.z, v.normal.x);
        float theta = asinf(-v.normal.y);
        v.texcoord.x = (phi + XM_PI) / XM_2PI;
        v.texcoord.y = (theta + XM_PIDIV2) / XM_PI;
    };

    Vertex top_vertex;
    top_vertex.position = DirectX::XMFLOAT3(0.0f, +radius, 0.0f);
    top_vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);
    top_vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
    top_vertex.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    top_vertex.binormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
    GetUV(top_vertex);

    Vertex bottom_vertex;
    bottom_vertex.position = DirectX::XMFLOAT3(0.0f, -radius, 0.0f);
    bottom_vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
    bottom_vertex.texcoord = DirectX::XMFLOAT2(0, 0);
    bottom_vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
    top_vertex.tangent = DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f);
    top_vertex.binormal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
    GetUV(bottom_vertex);



    mVertices.emplace_back(top_vertex);

    float phi_step = DirectX::XM_PI / stacks;
    float theta_step = 2.0f*DirectX::XM_PI / slices;

    // Compute vertices for each stack ring (do not count the poles as rings).
    for (uint32_t i = 1; i <= stacks - 1; ++i)
    {
        float phi = i * phi_step;

        // Vertices of ring.
        for (uint32_t j = 0; j <= slices; ++j)
        {
            float theta = j*theta_step;

            Vertex v;

            // spherical to cartesian
            v.position.x = radius * sinf(phi) * cosf(theta);
            v.position.y = radius * cosf(phi);
            v.position.z = radius * sinf(phi) * sinf(theta);

            Vector3 p = v.position;
            p.normalize();
            //DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));
            v.normal = p;
            GetUV(v);
            v.color = DirectX::XMFLOAT4(1, 1, 1, 1);
            Vector3 tan(-v.normal.z, 0.0f, v.normal.x);
            tan.normalize();
            v.tangent = DirectX::XMFLOAT3(tan.x, 0.0f, tan.z);
            Vector3 binormal = tan.cross(p);
            v.binormal = binormal;

            if (j == slices / 2)
            {
                v.texcoord.x = 1.0f;

                mVertices.emplace_back(v);
                v.texcoord.x = 0.0f;
            }

            mVertices.emplace_back(v);
        }
    }

    mVertices.emplace_back(bottom_vertex);

    //
    // Compute indices for top stack.  The top stack was written first to the vertex buffer
    // and connects the top pole to the first ring.
    //
    for (uint32_t i = 1; i <= slices + 1; ++i)
    {
        mIndices.emplace_back(0);
        mIndices.emplace_back(i + 1);
        mIndices.emplace_back(i);
    }

    //
    // Compute indices for inner stacks (not connected to poles).
    //

    // Offset the indices to the index of the first vertex in the first ring.
    // This is just skipping the top pole vertex.
    uint32_t base_index = 1;
    uint32_t ring_vertex_count = slices + 2;
    for (uint32_t i = 0; i < stacks - 2; ++i)
    {
        for (uint32_t j = 0; j < slices + 1; ++j)
        {
            mIndices.emplace_back(base_index + i*ring_vertex_count + j);
            mIndices.emplace_back(base_index + i*ring_vertex_count + j + 1);
            mIndices.emplace_back(base_index + (i + 1)*ring_vertex_count + j);

            mIndices.emplace_back(base_index + (i + 1)*ring_vertex_count + j);
            mIndices.emplace_back(base_index + i*ring_vertex_count + j + 1);
            mIndices.emplace_back(base_index + (i + 1)*ring_vertex_count + j + 1);
        }
    }

    //
    // Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
    // and connects the bottom pole to the bottom ring.
    //

    // South pole vertex was added last.
    uint32_t south_pole_index = (uint32_t)mVertices.size() - 1;

    // Offset the indices to the index of the first vertex in the last ring.
    base_index = south_pole_index - ring_vertex_count;

    for (uint32_t i = 0; i < slices + 1; ++i)
    {
        mIndices.emplace_back(south_pole_index);
        mIndices.emplace_back(base_index + i);
        mIndices.emplace_back(base_index + i + 1);
    }

    CreateBuffers(p_device);
}





//******************************************************************************
//
//      BasicCapsule
//
//******************************************************************************
BasicCapsule::BasicCapsule(
    Graphics::GraphicsDevice* p_device,
    unsigned int slices, 
    unsigned int stacks, 
    float radius,
    float height)
    :GeometricPrimitiveSelf(p_device)
{
    //std::vector<Vertex> vertices;
    //std::vector<WORD> indices;

    Vertex top_vertex;
    top_vertex.position = DirectX::XMFLOAT3(0.0f, +radius + height *0.5f, 0.0f);
    top_vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);
    top_vertex.texcoord = DirectX::XMFLOAT2(0, 0);
    top_vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);

    Vertex bottom_vertex;
    bottom_vertex.position = DirectX::XMFLOAT3(0.0f, -radius - height *0.5f, 0.0f);
    bottom_vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
    bottom_vertex.texcoord = DirectX::XMFLOAT2(0, 0);
    bottom_vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);

    mVertices.emplace_back(top_vertex);

    float phi_step = DirectX::XM_PI / stacks / 2;
    float theta_step = 2.0f * DirectX::XM_PI / slices;

    // Compute vertices for each stack ring (do not count the poles as rings).
    for (uint32_t i = 1; i <= stacks * 2 - 1; ++i)
    {
        float phi = i * phi_step;
        Vertex v;

        float yBase = height * 0.5f;
        if (i <= stacks)
        {
            // Vertices of ring.
            for (uint32_t j = 0; j <= slices; ++j)
            {
                float theta = j * theta_step;


                // spherical to cartesian
                v.position.x = radius * sinf(phi) * cosf(theta);
                v.position.y = radius * cosf(phi) + yBase;
                v.position.z = radius * sinf(phi) * sinf(theta);

                DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);
                DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));
                v.texcoord = DirectX::XMFLOAT2(0, 0);
                v.color = DirectX::XMFLOAT4(1, 1, 1, 1);

                mVertices.emplace_back(v);
            }

        }
        if (i >= stacks)
        {
            // Vertices of ring.
            for (uint32_t j = 0; j <= slices; ++j)
            {
                float theta = j * theta_step;


                // spherical to cartesian
                v.position.x = radius * sinf(phi) * cosf(theta);
                v.position.y = radius * cosf(phi) - yBase;
                v.position.z = radius * sinf(phi) * sinf(theta);

                DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);
                DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));
                v.texcoord = DirectX::XMFLOAT2(0, 0);
                v.color = DirectX::XMFLOAT4(1, 1, 1, 1);

                mVertices.emplace_back(v);
            }

        }


    }


    mVertices.emplace_back(bottom_vertex);

    //
    // Compute indices for top stack.  The top stack was written first to the vertex buffer
    // and connects the top pole to the first ring.
    //
    for (uint32_t i = 1; i <= slices; ++i)
    {
        mIndices.emplace_back(0);
        mIndices.emplace_back(i + 1);
        mIndices.emplace_back(i);
    }

    //
    // Compute indices for inner stacks (not connected to poles).
    //

    // Offset the indices to the index of the first vertex in the first ring.
    // This is just skipping the top pole vertex.
    uint32_t base_index = 1;
    uint32_t ring_vertex_count = slices + 1;
    for (uint32_t i = 0; i < stacks * 2 - 1; ++i)
    {
        for (uint32_t j = 0; j < slices; ++j)
        {
            mIndices.emplace_back(base_index + i * ring_vertex_count + j);
            mIndices.emplace_back(base_index + i * ring_vertex_count + j + 1);
            mIndices.emplace_back(base_index + (i + 1) * ring_vertex_count + j);

            mIndices.emplace_back(base_index + (i + 1) * ring_vertex_count + j);
            mIndices.emplace_back(base_index + i * ring_vertex_count + j + 1);
            mIndices.emplace_back(base_index + (i + 1) * ring_vertex_count + j + 1);
        }
    }

    //
    // Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
    // and connects the bottom pole to the bottom ring.
    //

    // South pole vertex was added last.
    uint32_t south_pole_index = (uint32_t)mVertices.size() - 1;

    // Offset the indices to the index of the first vertex in the last ring.
    base_index = south_pole_index - ring_vertex_count;

    for (uint32_t i = 0; i < slices; ++i)
    {
        mIndices.emplace_back(south_pole_index);
        mIndices.emplace_back(base_index + i);
        mIndices.emplace_back(base_index + i + 1);
    }

    CreateBuffers(p_device);

}

