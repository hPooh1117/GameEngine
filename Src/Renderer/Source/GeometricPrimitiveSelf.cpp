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
    D3D::DevicePtr& device,
    D3D11_PRIMITIVE_TOPOLOGY topology
    ) : Mesh()
{
    HRESULT hr = S_OK;

    mTopologyType = topology;

    //--> Create Constant Buffer
    hr = device->CreateBuffer(
        &CD3D11_BUFFER_DESC(
            sizeof(CBufferForMesh),
            D3D11_BIND_CONSTANT_BUFFER,
            D3D11_USAGE_DEFAULT,
            0,
            0,
            0
            ),
        nullptr,
        m_pConstantBufferMesh.GetAddressOf()
        );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    hr = device->CreateBuffer(
        &CD3D11_BUFFER_DESC(
            sizeof(CBufferForMaterial),
            D3D11_BIND_CONSTANT_BUFFER,
            D3D11_USAGE_DEFAULT,
            0,
            0,
            0
        ),
        nullptr,
        m_pConstantBufferMaterial.GetAddressOf()
    );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));




    //-->Create RasterizerState
    hr = device->CreateRasterizerState(
        &CD3D11_RASTERIZER_DESC(
            D3D11_FILL_WIREFRAME,     // D3D11_FILL_MODE FillMode;
            D3D11_CULL_BACK,          // D3D11_CULL_MODE CullMode;
            FALSE,                    // BOOL FrontCounterClockwise;
            0,                        // INT DepthBias;
            0.0f,                     // FLOAT DepthBiasClamp;
            0.0f,                     // FLOAT SlopeScaledDepthBias;
            TRUE,                     // BOOL DepthClipEnable;
            FALSE,                    // BOOL ScissorEnable;
            FALSE,                    // BOOL MultisampleEnable;
            FALSE                      // BOOL AntialiasedLineEnable;
            ),
        m_pRasterizerWire.GetAddressOf()
        );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    hr = device->CreateRasterizerState(
        &CD3D11_RASTERIZER_DESC(
            D3D11_FILL_SOLID,         // D3D11_FILL_MODE FillMode;
            D3D11_CULL_BACK,          // D3D11_CULL_MODE CullMode;
            FALSE,                    // BOOL FrontCounterClockwise;
            0,                        // INT DepthBias;
            0.0f,                     // FLOAT DepthBiasClamp;
            0.0f,                     // FLOAT SlopeScaledDepthBias;
            TRUE,                    // BOOL DepthClipEnable;
            FALSE,                    // BOOL ScissorEnable;
            FALSE,                    // BOOL MultisampleEnable;
            FALSE                      // BOOL AntialiasedLineEnable;
            ),
        m_pRasterizerSolid.GetAddressOf()
        );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));


}

GeometricPrimitiveSelf::~GeometricPrimitiveSelf()
{
}



void GeometricPrimitiveSelf::CreateBuffers(D3D::DevicePtr& device)
{
    HRESULT hr = S_OK;

    //--> Create Vertex Buffer
    D3D11_SUBRESOURCE_DATA vSrData = {};
    vSrData.pSysMem = mVertices.data();
    vSrData.SysMemPitch = 0;
    vSrData.SysMemSlicePitch = 0;

    //hr = pDev->CreateBuffer(&vertBufDesc, &vSrData, mVertexBuffer.GetAddressOf());
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
    hr = device->CreateBuffer(
        &CD3D11_BUFFER_DESC(
            sizeof(Vertex) * mVertices.size(),
            D3D11_BIND_VERTEX_BUFFER,
            D3D11_USAGE_IMMUTABLE,
            0,
            0,
            0
        ),
        &vSrData,
        m_pVertexBuffer.GetAddressOf()
    );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));



    D3D11_SUBRESOURCE_DATA iSrData = {};
    iSrData.pSysMem = mIndices.data();
    iSrData.SysMemPitch = 0;
    iSrData.SysMemSlicePitch = 0;

    //hr = pDev->CreateBuffer(&idxBuffDesc, &iSrData, mIndexBuffer.GetAddressOf());
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    hr = device->CreateBuffer(
        &CD3D11_BUFFER_DESC(
            sizeof(u_int) * mIndices.size(),
            D3D11_BIND_INDEX_BUFFER,
            D3D11_USAGE_IMMUTABLE,
            0,
            0,
            0
        ),
        &iSrData,
        m_pIndexBuffer.GetAddressOf()
    );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

}



void GeometricPrimitiveSelf::Render(
    D3D::DeviceContextPtr& imm_context,
    float elapsed_time,
    const DirectX::XMMATRIX& world,
    CameraController* camera,
    Shader* shader,
    const MaterialData& mat_data,
    bool isShadow,
    bool isSolid
)
{
    HRESULT hr = S_OK;

    if (shader != nullptr) shader->ActivateShaders(imm_context);

    SetRenderState(imm_context);

    XMFLOAT4X4 W, WVP;
    XMStoreFloat4x4(&W, world);
    if (isShadow)
    {
        XMStoreFloat4x4(&WVP, world * camera->GetOrthoView() * camera->GetOrthoProj(imm_context));
    }
    else
    {
        XMStoreFloat4x4(&WVP, world * camera->GetViewMatrix() * camera->GetProjMatrix(imm_context));
    }

    CBufferForMesh meshData = {};
    meshData.WVP = WVP;
    meshData.world = W;
    XMStoreFloat4x4(&meshData.invProj, camera->GetInvProjViewMatrix(imm_context));

    CBufferForMaterial matData = {};
    matData.mat_color = mat_data.mat_color;
    matData.metalness = mat_data.metalness;
    matData.roughness = mat_data.roughness;
    matData.specularColor = mat_data.specularColor;
    matData.textureConfig = mat_data.textureConfig;
    matData.diffuse = mat_data.diffuse;
    matData.specular = mat_data.specular;

    imm_context->UpdateSubresource(m_pConstantBufferMesh.Get(), 0, nullptr, &meshData, 0, 0);
    imm_context->VSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
    imm_context->HSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
    imm_context->DSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
    imm_context->GSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
    imm_context->PSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());

    imm_context->UpdateSubresource(m_pConstantBufferMaterial.Get(), 0, nullptr, &matData, 0, 0);
    imm_context->VSSetConstantBuffers(1, 1, m_pConstantBufferMaterial.GetAddressOf());
    imm_context->HSSetConstantBuffers(1, 1, m_pConstantBufferMaterial.GetAddressOf());
    imm_context->DSSetConstantBuffers(1, 1, m_pConstantBufferMaterial.GetAddressOf());
    imm_context->GSSetConstantBuffers(1, 1, m_pConstantBufferMaterial.GetAddressOf());
    imm_context->PSSetConstantBuffers(1, 1, m_pConstantBufferMaterial.GetAddressOf());


    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    imm_context->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
    imm_context->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    imm_context->IASetPrimitiveTopology(mTopologyType);

    isSolid ? imm_context->RSSetState(m_pRasterizerSolid.Get())
            : imm_context->RSSetState(m_pRasterizerWire.Get());

    D3D11_BUFFER_DESC bufDesc = {};
    m_pIndexBuffer->GetDesc(&bufDesc);
    imm_context->DrawIndexed(bufDesc.ByteWidth / sizeof(WORD), 0, 0);
}

//******************************************************************************
//
//      BasicLine
//
//******************************************************************************
BasicLine::BasicLine(D3D::DevicePtr& device)
    :GeometricPrimitiveSelf(device, D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
{

    Vertex vertices[] =
    {
        { XMFLOAT3(-0.5, +0.0, +0.0), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1)},
        { XMFLOAT3(+0.5, +0.0, +0.0), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1)},
    };
    WORD indices[] =
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


    CreateBuffers(device/*, vertices, indices, ARRAYSIZE(vertices), ARRAYSIZE(indices)*/);


    //--> Create Constant Buffer
    auto hr = device->CreateBuffer(
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

    if (m_pRasterizerSolid) m_pRasterizerSolid.Reset();
    hr = device->CreateRasterizerState(
        &CD3D11_RASTERIZER_DESC(
            D3D11_FILL_SOLID,         // D3D11_FILL_MODE FillMode;
            D3D11_CULL_NONE,          // D3D11_CULL_MODE CullMode;
            FALSE,                    // BOOL FrontCounterClockwise;
            0,                        // INT DepthBias;
            0.0f,                     // FLOAT DepthBiasClamp;
            0.0f,                     // FLOAT SlopeScaledDepthBias;
            TRUE,                    // BOOL DepthClipEnable;
            FALSE,                    // BOOL ScissorEnable;
            FALSE,                    // BOOL MultisampleEnable;
            FALSE                     // BOOL AntialiasedLineEnable;
        ),
        m_pRasterizerSolid.GetAddressOf()
    );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

}

void BasicLine::SetRenderState(D3D::DeviceContextPtr& imm_context)
{
    CBufferForLine data;
    data.param.x = 4;
    imm_context->UpdateSubresource(mConstantBufferForLine.Get(), 0, nullptr, &data, 0, 0);
    imm_context->VSSetConstantBuffers(1, 1, mConstantBufferForLine.GetAddressOf());
}


//******************************************************************************
//
//      BasicCube
//
//******************************************************************************
BasicCube::BasicCube(
    D3D::DevicePtr& device)
    :GeometricPrimitiveSelf(device)
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
    u_int indices[] = {
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

    CreateBuffers(device/*, vertices, indices, ARRAYSIZE(vertices), ARRAYSIZE(indices)*/);

}




//******************************************************************************
//
//      BasicCylinder
//
//******************************************************************************
BasicCylinder::BasicCylinder(
    D3D::DevicePtr& device,
    unsigned int slices) 
    :GeometricPrimitiveSelf(device)
{
    //std::vector<Vertex> vertices;
    //std::vector<WORD> indices;

    float d = 2.0f*DirectX::XM_PI / slices;
    float r = 0.5f;

    Vertex vertex;
    u_int base_index = 0;

    // top cap centre
    vertex.position = DirectX::XMFLOAT3(0.0f, +0.5f, 0.0f);
    vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);
    vertex.texcoord = DirectX::XMFLOAT2(0, 0);
    vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
    //vertex.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    //vertex.binormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
    mVertices.emplace_back(vertex);
    // top cap ring
    for (u_int i = 0; i < slices; ++i)
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
    for (u_int i = 0; i < slices - 1; ++i)
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
    for (u_int i = 0; i < slices; ++i)
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
    for (u_int i = 0; i < slices - 1; ++i)
    {
        mIndices.emplace_back(base_index + 0);
        mIndices.emplace_back(base_index + i + 1);
        mIndices.emplace_back(base_index + i + 2);
    }
    mIndices.emplace_back(base_index + 0);
    mIndices.emplace_back(base_index + (slices - 1) + 1);
    mIndices.emplace_back(base_index + (0) + 1);

    // side rectangle
    for (u_int i = 0; i < slices; ++i)
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
    for (u_int i = 0; i < slices - 1; ++i)
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


    CreateBuffers(device/*, vertices.data(), indices.data(), vertices.size(), indices.size()*/);


}



//******************************************************************************
//
//      BasicSphere
//
//******************************************************************************
BasicSphere::BasicSphere(
    D3D::DevicePtr& device,
    unsigned int slices, 
    unsigned int stacks,
    float radius)
    :GeometricPrimitiveSelf(device/*, D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST*/)
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
    for (u_int i = 1; i <= stacks - 1; ++i)
    {
        float phi = i * phi_step;

        // Vertices of ring.
        for (u_int j = 0; j <= slices; ++j)
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
    for (u_int i = 1; i <= slices + 1; ++i)
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
    u_int base_index = 1;
    u_int ring_vertex_count = slices + 2;
    for (u_int i = 0; i < stacks - 2; ++i)
    {
        for (u_int j = 0; j < slices + 1; ++j)
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
    u_int south_pole_index = (u_int)mVertices.size() - 1;

    // Offset the indices to the index of the first vertex in the last ring.
    base_index = south_pole_index - ring_vertex_count;

    for (u_int i = 0; i < slices + 1; ++i)
    {
        mIndices.emplace_back(south_pole_index);
        mIndices.emplace_back(base_index + i);
        mIndices.emplace_back(base_index + i + 1);
    }

    CreateBuffers(device/*, vertices.data(), indices.data(), vertices.size(), indices.size()*/);
}





//******************************************************************************
//
//      BasicCapsule
//
//******************************************************************************
BasicCapsule::BasicCapsule(
    D3D::DevicePtr& device,
    unsigned int slices, 
    unsigned int stacks, 
    float radius,
    float height)
    :GeometricPrimitiveSelf(device)
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
    for (u_int i = 1; i <= stacks * 2 - 1; ++i)
    {
        float phi = i * phi_step;
        Vertex v;

        float yBase = height * 0.5f;
        if (i <= stacks)
        {
            // Vertices of ring.
            for (u_int j = 0; j <= slices; ++j)
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
            for (u_int j = 0; j <= slices; ++j)
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
    for (u_int i = 1; i <= slices; ++i)
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
    u_int base_index = 1;
    u_int ring_vertex_count = slices + 1;
    for (u_int i = 0; i < stacks * 2 - 1; ++i)
    {
        for (u_int j = 0; j < slices; ++j)
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
    u_int south_pole_index = (u_int)mVertices.size() - 1;

    // Offset the indices to the index of the first vertex in the last ring.
    base_index = south_pole_index - ring_vertex_count;

    for (u_int i = 0; i < slices; ++i)
    {
        mIndices.emplace_back(south_pole_index);
        mIndices.emplace_back(base_index + i);
        mIndices.emplace_back(base_index + i + 1);
    }

    CreateBuffers(device/*, vertices.data(), indices.data(), vertices.size(), indices.size()*/);

}

