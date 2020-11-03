#include "Skybox.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/Light.h"

#include "./Utilities/misc.h"
#include "./Utilities/ImGuiSelf.h"

using namespace DirectX;

const wchar_t* Skybox::SKYBOX_TEXTURE[ENUM_SKYBOXID_MAX] = {
    L"Footprint_Court/Footprint_Court_8k_TMap.jpg",
    L"Old_Industrial_Hall/fin4_preview.jpg",
    L"Ridgecrest_Road/Ridgecrest_Road_4k_Bg.jpg",
    L"Serpentine_Valley/Serpentine_Valley_8k.jpg",
    L"Tokyo_BigSight/Tokyo_BigSight_8k.jpg",
    L"Ueno-Shrine/Ueno-Shrine_8k.jpg",
    L"Walk_Of_Fame/Mans_Outside_8k_TMap.jpg",
};
const std::wstring Skybox::TEXTURE_PATH = L"./Data/Images/Environment/";

Skybox::Skybox(
    Microsoft::WRL::ComPtr<ID3D11Device>& device,
    const wchar_t* filename,
    unsigned int slices, 
    unsigned int stacks, 
    float radius
):Mesh(),
mCurrentID(SkyboxTextureID::EFootprintCourt)
{
    HRESULT hr = S_OK;


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





    //-->Create RasterizerState
    hr = device->CreateRasterizerState(
        &CD3D11_RASTERIZER_DESC(
            D3D11_FILL_WIREFRAME,     // D3D11_FILL_MODE FillMode;
            D3D11_CULL_FRONT,          // D3D11_CULL_MODE CullMode;
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
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    hr = device->CreateRasterizerState(
        &CD3D11_RASTERIZER_DESC(
            D3D11_FILL_SOLID,         // D3D11_FILL_MODE FillMode;
            D3D11_CULL_FRONT,          // D3D11_CULL_MODE CullMode;
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
    
    FLOAT borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    hr = device->CreateSamplerState(
        &CD3D11_SAMPLER_DESC(
            D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_TEXTURE_ADDRESS_CLAMP,
            D3D11_TEXTURE_ADDRESS_CLAMP,
            D3D11_TEXTURE_ADDRESS_CLAMP,
            0,
            16,
            D3D11_COMPARISON_ALWAYS,
            borderColor,
            0,
            D3D11_FLOAT32_MAX
        ),
        mpClampSampler.GetAddressOf()
    );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));


    for (auto i = 0; i < SkyboxTextureID::ENUM_SKYBOXID_MAX; ++i)
    {
        mpTextures[i] = std::make_unique<NewTexture>();
        mpTextures[i]->Load(device, (TEXTURE_PATH + std::wstring(SKYBOX_TEXTURE[i])).c_str());
    }

    //mpTexture = std::make_unique<Texture>();
    //mpTexture->Load(device, filename);

#pragma region OLDIMPL
    //
    // Compute the vertices stating at the top pole and moving down the stacks.
    //

    // Poles: note that there will be texture coordinate distortion as there is
    // not a unique point on the texture map to assign to the pole when mapping
    // a rectangular texture onto a sphere.
    //auto GetUV = [&](Vertex& v)
    //{
    //    float phi = atan2f(v.normal.z, v.normal.x);
    //    float theta = asinf(-v.normal.y);
    //    v.texcoord.x = 1.0f - (phi + XM_PI) / XM_2PI;
    //    v.texcoord.y = (theta + XM_PIDIV2) / XM_PI;
    //};

    //Vertex top_vertex;
    //top_vertex.position = DirectX::XMFLOAT3(0.0f, +radius, 0.0f);
    //top_vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);
    //top_vertex.texcoord = DirectX::XMFLOAT2(0, 0);
    //top_vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
    //GetUV(top_vertex);

    //Vertex bottom_vertex;
    //bottom_vertex.position = DirectX::XMFLOAT3(0.0f, -radius, 0.0f);
    //bottom_vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
    //bottom_vertex.texcoord = DirectX::XMFLOAT2(0, 0);
    //bottom_vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
    //GetUV(bottom_vertex);



    //mVertices.emplace_back(top_vertex);

    //float phi_step = DirectX::XM_PI / stacks;
    //float theta_step = 2.0f * DirectX::XM_PI / slices;

    //// Compute vertices for each stack ring (do not count the poles as rings).
    //for (u_int i = 1; i <= stacks - 1; ++i)
    //{
    //    float phi = i * phi_step;

    //    // Vertices of ring.
    //    for (u_int j = 0; j <= slices; ++j)
    //    {
    //        float theta = j * theta_step;

    //        Vertex v;

    //        // spherical to cartesian
    //        v.position.x = radius * sinf(phi) * cosf(theta);
    //        v.position.y = radius * cosf(phi);
    //        v.position.z = radius * sinf(phi) * sinf(theta);

    //        Vector3 p = v.position;
    //        p.normalize();
    //        //DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));
    //        v.normal = p;
    //        GetUV(v);
    //        v.color = DirectX::XMFLOAT4(1, 1, 1, 1);
    //        if (j == slices / 2)
    //        {
    //            v.texcoord.x = 0.0f;

    //            mVertices.emplace_back(v);
    //            v.texcoord.x = 1.0f;
    //        }

    //        mVertices.emplace_back(v);


    //    }
    //}

    //mVertices.emplace_back(bottom_vertex);

    ////
    //// Compute indices for top stack.  The top stack was written first to the vertex buffer
    //// and connects the top pole to the first ring.
    ////
    //for (u_int i = 1; i <= slices + 1; ++i)
    //{
    //    mIndices.emplace_back(0);
    //    mIndices.emplace_back(i + 1);
    //    mIndices.emplace_back(i);
    //}

    ////
    //// Compute indices for inner stacks (not connected to poles).
    ////

    //// Offset the indices to the index of the first vertex in the first ring.
    //// This is just skipping the top pole vertex.
    //u_int base_index = 1;
    //u_int ring_vertex_count = slices + 2;
    //for (u_int i = 0; i < stacks - 2; ++i)
    //{
    //    for (u_int j = 0; j < slices + 1; ++j)
    //    {
    //        mIndices.emplace_back(base_index + i * ring_vertex_count + j);
    //        mIndices.emplace_back(base_index + i * ring_vertex_count + j + 1);
    //        mIndices.emplace_back(base_index + (i + 1) * ring_vertex_count + j);

    //        mIndices.emplace_back(base_index + (i + 1) * ring_vertex_count + j);
    //        mIndices.emplace_back(base_index + i * ring_vertex_count + j + 1);
    //        mIndices.emplace_back(base_index + (i + 1) * ring_vertex_count + j + 1);
    //    }
    //}

    ////
    //// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
    //// and connects the bottom pole to the bottom ring.
    ////

    //// South pole vertex was added last.
    //u_int south_pole_index = (u_int)mVertices.size() - 1;

    //// Offset the indices to the index of the first vertex in the last ring.
    //base_index = south_pole_index - ring_vertex_count;

    //for (u_int i = 0; i < slices + 1; ++i)
    //{
    //    mIndices.emplace_back(south_pole_index);
    //    mIndices.emplace_back(base_index + i);
    //    mIndices.emplace_back(base_index + i + 1);
    //}
#pragma endregion

    Vertex vertices[] = {
    { XMFLOAT3(-0.5, +0.5, +0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, +0.5, +0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(-0.5, +0.5, -0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, +0.5, -0.5), XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1) },


    { XMFLOAT3(-0.5, -0.5, +0.5), XMFLOAT3(+0.0f, -1.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, -0.5, +0.5), XMFLOAT3(+0.0f, -1.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(-0.5, -0.5, -0.5), XMFLOAT3(+0.0f, -1.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, -0.5, -0.5), XMFLOAT3(+0.0f, -1.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1) },


    { XMFLOAT3(-0.5, +0.5, -0.5), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, +0.5, -0.5), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(-0.5, -0.5, -0.5), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, -0.5, -0.5), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1) },


    { XMFLOAT3(-0.5, +0.5, +0.5), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, +0.5, +0.5), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(-0.5, -0.5, +0.5), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, -0.5, +0.5), XMFLOAT3(+0.0f, +0.0f, +1.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1) },


    { XMFLOAT3(+0.5, +0.5, -0.5), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, +0.5, +0.5), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, -0.5, -0.5), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(+0.5, -0.5, +0.5), XMFLOAT3(+1.0f, +0.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1) },


    { XMFLOAT3(-0.5, +0.5, -0.5), XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(-0.5, +0.5, +0.5), XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(-0.5, -0.5, -0.5), XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1) },
    { XMFLOAT3(-0.5, -0.5, +0.5), XMFLOAT3(-1.0f, +0.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT4(1, 1, 1, 1) },
    };

    for (auto& vertex : vertices)
    {
        vertex.position.x *= radius;
        vertex.position.y *= radius;
        vertex.position.z *= radius;
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


    CreateBuffers(device/*, vertices.data(), indices.data(), vertices.size(), indices.size()*/);

}


void Skybox::CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device)
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
        mpVertexBuffer.GetAddressOf()
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
        mpIndexBuffer.GetAddressOf()
    );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

}

void Skybox::Render(
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

    if (shader != nullptr) shader->activateShaders(imm_context);

    if (mCurrentID < 0 || mCurrentID >= SkyboxTextureID::ENUM_SKYBOXID_MAX) mCurrentID = 0;
    mpTextures[mCurrentID]->Set(imm_context, 0);

    XMFLOAT4X4 W, WVP;
    XMStoreFloat4x4(&W, world);
    XMStoreFloat4x4(&WVP, world * camera->GetViewMatrix() * camera->GetProjMatrix(imm_context));

    CBufferForMesh meshData = {};
    meshData.WVP = WVP;
    meshData.world = W;
    
    XMStoreFloat4x4(&meshData.invProj, camera->GetInvProjViewMatrix(imm_context));

    imm_context->UpdateSubresource(m_pConstantBufferMesh.Get(), 0, nullptr, &meshData, 0, 0);
    imm_context->VSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
    imm_context->PSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
    imm_context->PSSetSamplers(0, 1, mpClampSampler.GetAddressOf());
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    imm_context->IASetVertexBuffers(0, 1, mpVertexBuffer.GetAddressOf(), &stride, &offset);
    imm_context->IASetIndexBuffer(mpIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    imm_context->IASetPrimitiveTopology(mTopologyType);

    isSolid ? imm_context->RSSetState(m_pRasterizerSolid.Get())
        : imm_context->RSSetState(m_pRasterizerWire.Get());

    D3D11_BUFFER_DESC bufDesc = {};
    mpIndexBuffer->GetDesc(&bufDesc);
    imm_context->DrawIndexed(bufDesc.ByteWidth / sizeof(u_int), 0, 0);

}

void Skybox::RenderUI()
{
    if (ImGui::TreeNode("Skybox"))
    {
        if (ImGui::Button("FootprintCourt"))    mCurrentID = SkyboxTextureID::EFootprintCourt;
        if (ImGui::Button("OldIndustrialHall")) mCurrentID = SkyboxTextureID::EOldIndustrialHall;
        if (ImGui::Button("RidgecrestRoad"))    mCurrentID = SkyboxTextureID::ERidgecrestRoad;
        if (ImGui::Button("SerpentineValley"))  mCurrentID = SkyboxTextureID::ESerpentineValley;
        if (ImGui::Button("TokyoBigSight"))     mCurrentID = SkyboxTextureID::ETokyoBigSight;
        if (ImGui::Button("UenoShrine"))        mCurrentID = SkyboxTextureID::EUenoShrine;
        if (ImGui::Button("WalkOfFame"))        mCurrentID = SkyboxTextureID::EWalkOfFame;
        

        ImGui::TreePop();
        ImGui::Separator();
    }
}

Skybox::~Skybox()
{
}
