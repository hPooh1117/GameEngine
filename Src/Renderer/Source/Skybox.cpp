#include "Skybox.h"
#include "ResourceManager.h"
#include "Shader.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"

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
    L"Ueno-Shrine/03-Ueno-Shrine_preview.jpg",
    L"Walk_Of_Fame/Mans_Outside_8k_TMap.jpg",
    L"DUMMY",
    L"Gray0.png",
    L"Gray1.png",
};
const std::wstring Skybox::TEXTURE_PATH = L"./Data/Images/Environment/";

Skybox::Skybox()
    :Mesh(),
    mCurrentID(SkyboxTextureID::EFootprintCourt),
    mpCS_Equirectangular2Cube(std::make_unique<Shader>()),
    mpVertexBuffer(std::make_unique<Graphics::GPUBuffer>()),
    mpIndexBuffer(std::make_unique<Graphics::GPUBuffer>())
{
}


bool Skybox::Initialize(Graphics::GraphicsDevice* p_device, const wchar_t* filename, float radius)
{
    HRESULT hr = S_OK;
    auto& pDevice = p_device->GetDevicePtr();

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





    ////-->Create RasterizerState
    //hr = pDevice->CreateRasterizerState(
    //    &CD3D11_RASTERIZER_DESC(
    //        D3D11_FILL_WIREFRAME,     // D3D11_FILL_MODE FillMode;
    //        D3D11_CULL_FRONT,          // D3D11_CULL_MODE CullMode;
    //        FALSE,                    // BOOL FrontCounterClockwise;
    //        0,                        // INT DepthBias;
    //        0.0f,                     // FLOAT DepthBiasClamp;
    //        0.0f,                     // FLOAT SlopeScaledDepthBias;
    //        TRUE,                    // BOOL DepthClipEnable;
    //        FALSE,                    // BOOL ScissorEnable;
    //        FALSE,                    // BOOL MultisampleEnable;
    //        FALSE                     // BOOL AntialiasedLineEnable;
    //    ),
    //    m_pRasterizerWire.GetAddressOf()
    //);
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    //hr = pDevice->CreateRasterizerState(
    //    &CD3D11_RASTERIZER_DESC(
    //        D3D11_FILL_SOLID,         // D3D11_FILL_MODE FillMode;
    //        D3D11_CULL_FRONT,          // D3D11_CULL_MODE CullMode;
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



    for (auto i = 0; i < SkyboxTextureID::SKYBOXID_MAX; ++i)
    {
        mpTextures[i] = std::make_unique<Texture>();
        if (i == EDUMMY)
        {
            mpTextures[i]->Load(pDevice);
            continue;
        }
        mpTextures[i]->Load(pDevice, (TEXTURE_PATH + std::wstring(SKYBOX_TEXTURE[i])).c_str());
    }
    for (auto i = 0; i < SkyboxTextureID::SKYBOXID_MAX; ++i)
    {
        mpComputedTexs[i] = std::make_unique<ComputedTexture>();
        mpComputedTexs[i]->CreateTextureCube(pDevice, 1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
        mpComputedTexs[i]->CreateTextureUAV(pDevice, 0);
        //mpComputedTexs[i]->CreateShader(device, L"./Src/Shaders/CS_Equirectangular2Cube.hlsl", "CSmain");
    }

    mpCS_Equirectangular2Cube->CreateComputeShader(pDevice, L"./Src/Shaders/CS_Equirectangular2Cube.hlsl", "CSmain");


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


    CreateBuffers(p_device);

    return true;
}

void Skybox::CreateBuffers(Graphics::GraphicsDevice* p_device)
{
    //HRESULT hr = S_OK;
    //auto& pDevice = p_device->GetDevicePtr();

    ////--> Create Vertex Buffer
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
    //    mpVertexBuffer.GetAddressOf()
    //);
    //_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));



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
    //    mpIndexBuffer.GetAddressOf()
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

void Skybox::Render(
    Graphics::GraphicsDevice* p_device,
    float elapsed_time,
    const DirectX::XMMATRIX& world,
    CameraController* camera,
    Shader* shader,
    const Material& mat_data,
    bool isShadow,
    bool isSolid )
{
    if (!mbIsDrawing) return;

    HRESULT hr = S_OK;
    auto& pImmContext = p_device->GetImmContextPtr();

    if (shader != nullptr) shader->ActivateShaders(pImmContext);

    if (mCurrentID < 0 || mCurrentID >= SkyboxTextureID::SKYBOXID_MAX) mCurrentID = 0;
    //mpTextures[mCurrentID]->Set(p_imm_context, 0);
    //if (!mbIsDrawing) mpTextures[EDUMMY]->Set(p_imm_context, 0);
    mpComputedTexs[mCurrentID]->Set(pImmContext, 13);
    if (!mbIsDrawing)
    {
        mpComputedTexs[EDUMMY]->Set(pImmContext, 13);
    }

    XMFLOAT4X4 W, WVP;
    XMStoreFloat4x4(&W, world);
    XMStoreFloat4x4(&WVP, world * camera->GetViewMatrix() * camera->GetProjMatrix(pImmContext));

    CBufferForMesh meshData = {};
    meshData.WVP = WVP;
    meshData.world = W;
    
    XMStoreFloat4x4(&meshData.invViewProj, camera->GetInvProjViewMatrix(pImmContext));
    DirectX::XMStoreFloat4x4(&meshData.invView, camera->GetInvViewMatrix());
    DirectX::XMStoreFloat4x4(&meshData.invProj, camera->GetInvProjMatrix(pImmContext));

    pImmContext->UpdateSubresource(mpConstantBufferMesh.Get(), 0, nullptr, &meshData, 0, 0);
    pImmContext->VSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
    pImmContext->PSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
    p_device->SetSamplers(Graphics::SS_LINEAR_CLAMP, 0);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    //pImmContext->IASetVertexBuffers(0, 1, mpVertexBuffer.GetAddressOf(), &stride, &offset);
    //pImmContext->IASetIndexBuffer(mpIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    //pImmContext->IASetPrimitiveTopology(mTopologyType);

    //isSolid ? pImmContext->RSSetState(m_pRasterizerSolid.Get())
    //        : pImmContext->RSSetState(m_pRasterizerWire.Get());
    Graphics::GPUBuffer* vb = mpVertexBuffer.get();
    p_device->BindVertexBuffer(&vb, 0, 1, &stride, &offset);
    p_device->BindIndexBuffer(mpIndexBuffer.get(), Graphics::IndexBufferFormat::INDEXFORMAT_32BIT, 0);
    p_device->BindPrimitiveTopology(mToplogyID);

    isSolid ? p_device->RSSetState(Graphics::RS_SKYBOX)
        : p_device->RSSetState(Graphics::RS_WIRE_FRONT);
    //p_device->OMSetDepthStencilState(Graphics::DS_TRUE);

    Graphics::GPUBufferDesc bufDesc = mpIndexBuffer->GetDesc();
    p_device->DrawIndexed(bufDesc.ByteWidth / sizeof(uint32_t), 0, 0);

}

void Skybox::ConvertEquirectToCubeMap(Graphics::GraphicsDevice* p_device)
{
    auto& ImmContext = p_device->GetImmContextPtr();
    for (auto i = 0; i < SKYBOXID_MAX; ++i)
    {
        p_device->SetSamplers(Graphics::SS_LINEAR_WRAP, 0);
        mpCS_Equirectangular2Cube->ActivateCSShader(ImmContext);
        mpComputedTexs[i]->Compute(p_device, mpTextures[i]->GetSRV(), 6);
        ImmContext->GenerateMips(mpComputedTexs[i]->GetSRV().Get());
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
