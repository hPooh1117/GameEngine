#include "Skybox.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"
#include "ComputedTexture.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/Light.h"

#include "./Utilities/misc.h"
#include "./Utilities/ImGuiSelf.h"

using namespace DirectX;

const wchar_t* Skybox::SKYBOX_TEXTURE[SKYBOXID_MAX] = {
    L"Footprint_Court/Footprint_Court_8k_TMap.jpg",
    //L"Footprint_Court/Footprint_Court_2k.hdr",
    //L"Old_Industrial_Hall/fin4_preview.jpg",
    L"Factory_Catwalk/Factory_Catwalk_Bg.jpg",
    L"Mono_Lake_C/Mono_Lake_C_HiRes.jpg",
    L"Tokyo_BigSight/Tokyo_BigSight_8k.jpg",
    L"Ueno-Shrine/Ueno-Shrine_8k.jpg",
    L"Walk_Of_Fame/Mans_Outside_8k_TMap.jpg",
    L"DUMMY",
    L"Gray0.png",
    L"Gray1.png",
};
const std::wstring Skybox::TEXTURE_PATH = L"./Data/Images/Environment/";

Skybox::Skybox()
    :Mesh(),
    mCurrentID(SkyboxTextureID::EFootprintCourt)
{
}


bool Skybox::Initialize(D3D::DevicePtr& device, const wchar_t* filename, float radius)
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
        mpConstantBufferMesh.GetAddressOf()
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


    for (auto i = 0; i < SkyboxTextureID::SKYBOXID_MAX; ++i)
    {
        mpTextures[i] = std::make_unique<Texture>();
        if (i == EDUMMY)
        {
            mpTextures[i]->Load(device);
            continue;
        }
        mpTextures[i]->Load(device, (TEXTURE_PATH + std::wstring(SKYBOX_TEXTURE[i])).c_str());
    }
    for (auto i = 0; i < SkyboxTextureID::SKYBOXID_MAX; ++i)
    {
        mpComputedTexs[i] = std::make_unique<ComputedTexture>();
        mpComputedTexs[i]->CreateTextureCube(device, 1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
        mpComputedTexs[i]->CreateTextureUAV(device, 0);
        mpComputedTexs[i]->CreateShader(device, L"./Src/Shaders/CS_Equirectangular2Cube.hlsl", "CSmain");
        mpComputedTexs[i]->CreateSampler(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
    }



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

    return true;
}

void Skybox::CreateBuffers(D3D::DevicePtr& p_device)
{
    HRESULT hr = S_OK;

    //--> Create Vertex Buffer
    D3D11_SUBRESOURCE_DATA vSrData = {};
    vSrData.pSysMem = mVertices.data();
    vSrData.SysMemPitch = 0;
    vSrData.SysMemSlicePitch = 0;

    //hr = pDev->CreateBuffer(&vertBufDesc, &vSrData, mVertexBuffer.GetAddressOf());
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
    hr = p_device->CreateBuffer(
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

    hr = p_device->CreateBuffer(
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
    D3D::DeviceContextPtr& p_imm_context,
    float elapsed_time,
    const DirectX::XMMATRIX& world,
    CameraController* camera,
    Shader* shader,
    const MaterialData& mat_data,
    bool isShadow,
    bool isSolid )
{
    //if (!mbIsDrawing) return;

    HRESULT hr = S_OK;

    if (shader != nullptr) shader->ActivateShaders(p_imm_context);

    if (mCurrentID < 0 || mCurrentID >= SkyboxTextureID::SKYBOXID_MAX) mCurrentID = 0;
    //mpTextures[mCurrentID]->Set(p_imm_context, 0);
    //if (!mbIsDrawing) mpTextures[EDUMMY]->Set(p_imm_context, 0);
    mpComputedTexs[mCurrentID]->Set(p_imm_context, 13);
    if (!mbIsDrawing)
    {
        mpComputedTexs[EDUMMY]->Set(p_imm_context, 13);
    }

    XMFLOAT4X4 W, WVP;
    XMStoreFloat4x4(&W, world);
    XMStoreFloat4x4(&WVP, world * camera->GetViewMatrix() * camera->GetProjMatrix(p_imm_context));

    CBufferForMesh meshData = {};
    meshData.WVP = WVP;
    meshData.world = W;
    
    XMStoreFloat4x4(&meshData.invViewProj, camera->GetInvProjViewMatrix(p_imm_context));
    DirectX::XMStoreFloat4x4(&meshData.invView, camera->GetInvViewMatrix());
    DirectX::XMStoreFloat4x4(&meshData.invProj, camera->GetInvProjMatrix(p_imm_context));

    p_imm_context->UpdateSubresource(mpConstantBufferMesh.Get(), 0, nullptr, &meshData, 0, 0);
    p_imm_context->VSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
    p_imm_context->PSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
    p_imm_context->PSSetSamplers(0, 1, mpClampSampler.GetAddressOf());
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    p_imm_context->IASetVertexBuffers(0, 1, mpVertexBuffer.GetAddressOf(), &stride, &offset);
    p_imm_context->IASetIndexBuffer(mpIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    p_imm_context->IASetPrimitiveTopology(mTopologyType);

    isSolid ? p_imm_context->RSSetState(m_pRasterizerSolid.Get())
        : p_imm_context->RSSetState(m_pRasterizerWire.Get());

    D3D11_BUFFER_DESC bufDesc = {};
    mpIndexBuffer->GetDesc(&bufDesc);
    p_imm_context->DrawIndexed(bufDesc.ByteWidth / sizeof(u_int), 0, 0);

}

void Skybox::ConvertEquirectToCubeMap(D3D::DeviceContextPtr& p_imm_context)
{
    for (auto i = 0; i < SKYBOXID_MAX; ++i)
    {
        mpComputedTexs[i]->Compute(p_imm_context, mpTextures[i]->GetSRV(), 6);
        p_imm_context->GenerateMips(mpComputedTexs[i]->GetSRV().Get());
    }
    mbHasComputed = true;
}

void Skybox::RenderUI()
{
    
    if (ImGui::TreeNode("Skybox"))
    {
        ImGui::Text("Skybox existence");
        if (ImGui::Button("ON")) mbIsDrawing = true;
        ImGui::SameLine();
        if (ImGui::Button("OFF")) mbIsDrawing = false;

        if (ImGui::Button("FootprintCourt"))    mCurrentID = SkyboxTextureID::EFootprintCourt;
        if (ImGui::Button("Factory_Catwalk"))   mCurrentID = SkyboxTextureID::EFactoryCatWalk;
        if (ImGui::Button("Mono_Lake_C"))       mCurrentID = SkyboxTextureID::EMonoLake;
        if (ImGui::Button("TokyoBigSight"))     mCurrentID = SkyboxTextureID::ETokyoBigSight;
        if (ImGui::Button("UenoShrine"))        mCurrentID = SkyboxTextureID::EUenoShrine;
        if (ImGui::Button("WalkOfFame"))        mCurrentID = SkyboxTextureID::EWalkOfFame;
        if (ImGui::Button("Gray0"))             mCurrentID = SkyboxTextureID::EGray0;
        if (ImGui::Button("Gray1"))             mCurrentID = SkyboxTextureID::EGray1;

        ImGui::TreePop();
        ImGui::Separator();
    }
}

Skybox::~Skybox()
{
}
