#include "Plane.h"
#include "./Utilities/misc.h"
#include <DirectXMath.h>
#include "Shader.h"
#include "./Engine/MainCamera.h"
#include "./Engine/Light.h"
#include "./Engine/CameraController.h"
#include "ResourceManager.h"
#include "Texture.h"

//--------------------------------------------------------------------------------------------------------------------------------


using namespace DirectX;


//********************************************************************************************************************************
//
//      Plane
//
//********************************************************************************************************************************
//--------------------------------------------------------------------------------------------------------------------------------

Plane::Plane(D3D::DevicePtr& device, const wchar_t* filename):Mesh()
{
	auto result = S_OK;

    Vertex vertices[] = {
    { XMFLOAT3(-0.5, +0.0, +0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, +0.0, +0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(-0.5, +0.0, -0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, +0.0, -0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1) },

    };

    for (auto& vertex : vertices)
    {
        mVertices.emplace_back(vertex);
    }

    u_int indices[] =
    {
        0, 1, 2, 2, 1, 3
    };
    for (auto& index : indices)
    {
        mIndices.emplace_back(index);
    }

    CreateBuffers(device/*, vertices, indices, ARRAYSIZE(vertices), ARRAYSIZE(indices)*/);

	result = device->CreateBuffer(
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
	_ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));


    result = device->CreateRasterizerState(
        &CD3D11_RASTERIZER_DESC(
            D3D11_FILL_WIREFRAME,     // D3D11_FILL_MODE FillMode;
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
        m_pRasterizerWire.GetAddressOf()
    );
    _ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));

    result = device->CreateRasterizerState(
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
    _ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));


    m_pTexture = std::make_unique<Texture>();
    if (filename != L"\0")
    {
        m_pTexture->Load(device, filename);
    }
    else
    {
        m_pTexture->Load(device);
    }


}

//--------------------------------------------------------------------------------------------------------------------------------

void Plane::CreateBuffers(D3D::DevicePtr& device)
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

//--------------------------------------------------------------------------------------------------------------------------------

void Plane::Render(
    D3D::DeviceContextPtr& imm_context,
    float elapsed_time, 
    const DirectX::XMMATRIX& world, 
    const std::shared_ptr<CameraController>& camera, 
    const std::shared_ptr<Shader>& shader,
    const DirectX::XMFLOAT4& mat_color,
    bool isShadow,
    bool isSolid)
{
    HRESULT hr = S_OK;

    shader->activateShaders(imm_context);

    XMFLOAT4X4 W, WVP;
    XMStoreFloat4x4(&W, world);
    XMStoreFloat4x4(&WVP, world * camera->GetViewMatrix() * camera->GetProjMatrix(imm_context));

    CBufferForMesh matData = {};
    matData.m_WVP = WVP;
    matData.m_world = W;

    imm_context->UpdateSubresource(m_pConstantBufferMesh.Get(), 0, nullptr, &matData, 0, 0);
    imm_context->VSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
    imm_context->PSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());


    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    imm_context->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
    imm_context->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    imm_context->IASetPrimitiveTopology(mTopologyType);

    isSolid ? imm_context->RSSetState(m_pRasterizerSolid.Get())
        : imm_context->RSSetState(m_pRasterizerWire.Get());

    m_pTexture->Set(imm_context);

    D3D11_BUFFER_DESC bufDesc = {};
    m_pIndexBuffer->GetDesc(&bufDesc);
    imm_context->DrawIndexed(bufDesc.ByteWidth / sizeof(WORD), 0, 0);

}

//--------------------------------------------------------------------------------------------------------------------------------

Plane::~Plane()
{
}

//--------------------------------------------------------------------------------------------------------------------------------


//********************************************************************************************************************************
//
//      PlaneBatch
//
//********************************************************************************************************************************
//--------------------------------------------------------------------------------------------------------------------------------

PlaneBatch::PlaneBatch(
    D3D::DevicePtr& device,
    const wchar_t* filename,
    const int max_instances)
    :Mesh(),
    MAX_INSTANCES(max_instances)
{
    HRESULT hr = S_OK;

    VertexForBatch vertices[] =
    {
        {XMFLOAT3(-0.5f, -0.5f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0, 0)},
        {XMFLOAT3(+0.5f, -0.5f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(1, 0)},
        {XMFLOAT3(-0.5f, +0.5f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0, 1)},
        {XMFLOAT3(+0.5f, +0.5f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(1, 1)}
    };
    
    for (auto& vertex : vertices)
    {
        mVertices.emplace_back(vertex);
    }

    CreateBuffers(device);

    D3D11_RASTERIZER_DESC rrDesc = {};
    rrDesc.AntialiasedLineEnable = FALSE;
    rrDesc.FillMode = D3D11_FILL_SOLID;
    rrDesc.CullMode = D3D11_CULL_NONE;
    rrDesc.FrontCounterClockwise = FALSE;
    rrDesc.DepthBias = 0;
    rrDesc.DepthBiasClamp = 0;
    rrDesc.SlopeScaledDepthBias = 0;
    rrDesc.DepthClipEnable = TRUE;
    rrDesc.ScissorEnable = FALSE;
    rrDesc.MultisampleEnable = FALSE;
    hr = device->CreateRasterizerState(&rrDesc, m_pRasterizerSolid.GetAddressOf());
    if (FAILED(hr)) return;

    m_pTexture = std::make_unique<Texture>();
    if (filename != L"\0")
    {
        m_pTexture->Load(device, filename);
    }
    else
    {
        m_pTexture->Load(device);
    }



}

//--------------------------------------------------------------------------------------------------------------------------------

void PlaneBatch::CreateBuffers(D3D::DevicePtr& device)
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
            sizeof(VertexForBatch) * mVertices.size(),
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

        hr = device->CreateBuffer(&ibDesc, &srData, m_instance_buffer.GetAddressOf());
        if (FAILED(hr)) return;
    }
    delete[] instances;

}

//--------------------------------------------------------------------------------------------------------------------------------

void PlaneBatch::Begin(D3D::DeviceContextPtr& imm_context, const std::shared_ptr<Shader>& shader
)
{
    HRESULT hr = S_OK;

    shader->activateShaders(imm_context);

    UINT strides[2] = { sizeof(VertexForBatch), sizeof(Instance) };
    UINT offsets[2] = { 0,0 };

    ID3D11Buffer* vbs[2] = { m_pVertexBuffer.Get(), m_instance_buffer.Get() };
    imm_context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
    imm_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    imm_context->RSSetState(m_pRasterizerSolid.Get());

    m_pTexture->Set(imm_context);


    D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
    D3D11_MAPPED_SUBRESOURCE mappedBuffer;
    hr = imm_context->Map(m_instance_buffer.Get(), 0, map, 0, &mappedBuffer);
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    m_instance = static_cast<Instance*>(mappedBuffer.pData);

    m_instances_count = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------

void PlaneBatch::Render(
    D3D::DeviceContextPtr& imm_context,
    float elapsed_time, 
    const DirectX::XMMATRIX& world, 
    const std::shared_ptr<CameraController>& camera, 
    const std::shared_ptr<Shader>& shader, 
    const DirectX::XMFLOAT4& mat_color, 
    bool isShadow,
    bool isSolid)
{
    _ASSERT_EXPR(m_instances_count < MAX_INSTANCES, L"Too Many Instances is EXISTING");

    shader->activateShaders(imm_context);

    DirectX::XMMATRIX view_proj = camera->GetViewMatrix() * camera->GetProjMatrix(imm_context);
    XMStoreFloat4x4(&m_instance[m_instances_count].ndc_transform, DirectX::XMMatrixTranspose(world * view_proj));

    //m_instance[m_instances_count].texcoord_transform = DirectX::XMFLOAT4(0, 0, 1, 1);
    m_instance[m_instances_count].color = mat_color;


    m_instances_count++;

}

//--------------------------------------------------------------------------------------------------------------------------------

void PlaneBatch::Render(
    D3D::DeviceContextPtr& imm_context,
    const DirectX::XMMATRIX& world,
    const std::shared_ptr<CameraController>& camera, 
    const DirectX::XMFLOAT4& mat_color,
    bool particle_mode)
{
    _ASSERT_EXPR(m_instances_count < MAX_INSTANCES, L"Too Many Instances is EXISTING");

    DirectX::XMMATRIX view_proj = camera->GetViewMatrix() * camera->GetProjMatrix(imm_context);
    XMMATRIX W = world;
    if (particle_mode) W = camera->GetInvViewMatrix() * world;
    XMStoreFloat4x4(&m_instance[m_instances_count].ndc_transform, DirectX::XMMatrixTranspose(W * view_proj));

    m_instance[m_instances_count].color = mat_color;


    m_instances_count++;
}

//--------------------------------------------------------------------------------------------------------------------------------

void PlaneBatch::End(D3D::DeviceContextPtr& imm_context)
{
    imm_context->Unmap(m_instance_buffer.Get(), 0);
    imm_context->DrawInstanced(4, m_instances_count, 0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------------

PlaneBatch::~PlaneBatch()
{
}

//--------------------------------------------------------------------------------------------------------------------------------
