#include "StaticMesh.h"

#include <fstream>

#include "ResourceManager.h"

#include "./Utilities/misc.h"

using namespace DirectX;

StaticMesh::StaticMesh(Graphics::GraphicsDevice* p_device, const wchar_t* OBJFile, bool isFlippingV)
    :Mesh(),
    mpVertexBuffer(std::make_unique<Graphics::GPUBuffer>()),
    mpIndexBuffer(std::make_unique<Graphics::GPUBuffer>())
{
    HRESULT hr = S_OK;
    auto& pDevice = p_device->GetDevicePtr();

    LoadOBJFile(p_device, OBJFile, isFlippingV);

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



    for (auto &material : mMaterials)
    {
        std::unique_ptr<Texture> tex = std::make_unique<Texture>();
        if (material.map_Kd == L"\0")
        {
            tex->Load(pDevice);
            continue;
        }
        tex->Load(pDevice, material.map_Kd.data());
        material.texture = std::move(tex);
    }




}

StaticMesh::~StaticMesh()
{
}


void StaticMesh::CreateBuffers(Graphics::GraphicsDevice* p_device)
{
    // VERTEX BUFFER
    Graphics::GPUBufferDesc desc = {};
    desc.ByteWidth = sizeof(Vertex) * mVertices.size();
    desc.Usage = Graphics::USAGE_IMMUTABLE;
    desc.BindFlags = Graphics::BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    mpVertexBuffer->desc = desc;

    Graphics::SubresourceData data = {};
    data.pSysMem = mVertices.data();
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;
    p_device->CreateBuffer(&desc, &data, mpVertexBuffer.get());

    // INDEX BUFFER
    desc.ByteWidth = sizeof(uint32_t) * mIndices.size();
    desc.BindFlags = Graphics::BIND_INDEX_BUFFER;
    mpIndexBuffer->desc = desc;
    data.pSysMem = mIndices.data();
    p_device->CreateBuffer(&desc, &data, mpIndexBuffer.get());

}

void StaticMesh::Render(
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

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    Graphics::GPUBuffer* vb = mpVertexBuffer.get();
    p_device->BindVertexBuffer(&vb, 0, 1, &stride, &offset);
    p_device->BindIndexBuffer(mpIndexBuffer.get(), Graphics::IndexBufferFormat::INDEXFORMAT_32BIT, 0);
    p_device->BindPrimitiveTopology(mToplogyID);

    isSolid ? p_device->RSSetState(Graphics::RS_BACK)
        : p_device->RSSetState(Graphics::RS_WIRE_BACK);

    CBufferForMesh meshData = {};
    DirectX::XMStoreFloat4x4(&meshData.world, world);
    DirectX::XMStoreFloat4x4(&meshData.WVP, world * (isShadow ? camera->GetOrthoView() * camera->GetOrthoProj(ImmContext) : camera->GetViewMatrix() * camera->GetProjMatrix(ImmContext)));
    XMStoreFloat4x4(&meshData.invViewProj, camera->GetInvProjViewMatrix(ImmContext));
    DirectX::XMStoreFloat4x4(&meshData.invView, camera->GetInvViewMatrix());
    DirectX::XMStoreFloat4x4(&meshData.invProj, camera->GetInvProjMatrix(ImmContext));



    for (auto &material : mMaterials)
    {

        CBufferForMaterial matData = {};
        matData.mat_color = mat_data.mat_color;
        matData.metalness = mat_data.metalness;
        matData.roughness = mat_data.roughness;
        matData.specularColor = mat_data.specularColor;
        matData.textureConfig = mat_data.textureConfig;
        matData.diffuse = mat_data.diffuse;
        matData.specular = mat_data.specular;

        ImmContext->UpdateSubresource(mpConstantBufferMesh.Get(), 0, nullptr, &meshData, 0, 0);
        ImmContext->UpdateSubresource(mpConstantBufferMaterial.Get(), 0, nullptr, &matData, 0, 0);

        ImmContext->VSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
        ImmContext->HSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
        ImmContext->DSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
        ImmContext->GSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
        ImmContext->PSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());

        ImmContext->VSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
        ImmContext->HSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
        ImmContext->DSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
        ImmContext->GSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
        ImmContext->PSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());

        material.texture->Set(ImmContext);

        for (auto &subset : mSubsets)
        {
            if (material.newmtl == subset.usemtl)
            {
                p_device->DrawIndexed(subset.index_count, subset.index_start, 0);
            }
        }
    }
}

void StaticMesh::LoadOBJFile(Graphics::GraphicsDevice* p_device, const wchar_t *filename, bool isFlippingV)
{
    uint32_t currentIndex = 0;

    auto pDevice = p_device->GetDevicePtr();

    std::vector<XMFLOAT3> positions;
    std::vector<XMFLOAT3> normals;
    std::vector<XMFLOAT2> texcoords;

    std::vector<std::wstring> mtl_filenames;

    std::wifstream file(filename);
    _ASSERT_EXPR_A(file, L"OBJ file was not found.");

    wchar_t command[256];
    Vector3 v0, v1, v2;

    Vector2 uv0, uv1, uv2;

    Vector3 deltaPos1, deltaPos2;

    Vector2 deltaUV1, deltaUV2;

    float r;
    Vector3 tangent, binormal;

    while (file)
    {
        file >> command;
        if (wcscmp(command, L"v") == 0)
        {
            float x, y, z;
            file >> x >> y >> z;
            positions.push_back(XMFLOAT3(x, y, z));
            file.ignore(1024, L'\n');
        }
        else if (wcscmp(command, L"vn") == 0)
        {
            FLOAT i, j, k;
            file >> i >> j >> k;
            normals.push_back(XMFLOAT3(i, j, k));
            file.ignore(1024, L'\n');
        }
        else if (wcscmp(command, L"vt") == 0)
        {
            FLOAT u, v;
            file >> u >> v;
            texcoords.push_back(XMFLOAT2(u, isFlippingV ? 1.0f - v : v));
            file.ignore(1024, L'\n');
        }
        else if (wcscmp(command, L"f") == 0)
        {
            Vertex vertices[3];
            for (auto i = 0; i < 3; ++i)
            {
                u_int v, vt, vn;

                file >> v;
                vertices[i].position = positions[v - 1];
                if (L'/' == file.peek())
                {
                    file.ignore();
                    if (L'/' != file.peek())
                    {
                        file >> vt;
                        vertices[i].texcoord = texcoords[vt - 1];
                    }
                    if (L'/' == file.peek())
                    {
                        file.ignore();
                        file >> vn;
                        vertices[i].normal = normals[vn - 1];
                    }
                    vertices[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                }
            }
            v0 = vertices[0].position;
            v1 = vertices[1].position;
            v2 = vertices[2].position;

            uv0 = vertices[0].texcoord;
            uv1 = vertices[1].texcoord;
            uv2 = vertices[2].texcoord;

            deltaPos1 = v1 - v0;
            deltaPos2 = v2 - v0;

            deltaUV1 = uv1 - uv0;
            deltaUV2 = uv2 - uv0;

            // position, texcoordÇ©ÇÁtangent, binormalÇÃéZèo
            r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)* r;
            binormal = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
            
            for (auto i = 0; i < 3; ++i)
            {
                mVertices.emplace_back(vertices[i]);
                mIndices.emplace_back(currentIndex++);
            }

            file.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"mtllib"))
        {
            wchar_t mtllib[256];
            file >> mtllib;
            mtl_filenames.push_back(mtllib);
        }
        else if (0 == wcscmp(command, L"usemtl"))
        {
            wchar_t usemtl[MAX_PATH] = { 0 };
            file >> usemtl;

            Subset current_subset = {};
            current_subset.usemtl = usemtl;
            current_subset.index_start = mIndices.size();
            mSubsets.emplace_back(current_subset);
        }
        else
        {
            file.ignore(1024, L'\n');
        }
    }

    std::vector<Subset>::reverse_iterator iterator = mSubsets.rbegin();
    iterator->index_count = mIndices.size() - iterator->index_start;
    for (iterator = mSubsets.rbegin() + 1; iterator != mSubsets.rend(); ++iterator)
    {
        iterator->index_count = (iterator - 1)->index_start - iterator->index_start;
    }


    LoadMTLFile(pDevice, filename, mtl_filenames);

    CreateBuffers(p_device);
}

void StaticMesh::LoadMTLFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t *objfilename, std::vector<std::wstring>& mtlFilenames)
{
    wchar_t mtlFilename[256];

    ResourceManager::CreateFilenameToRefer(mtlFilename, objfilename, mtlFilenames[0].c_str());

    std::wifstream file(mtlFilename);

    if (!file.is_open())
    {
        for (auto& subset : mSubsets)
        {
            mMaterials.emplace_back(MaterialData());
            mMaterials.back().map_Kd = L"\0";
        }
    }

    wchar_t command[256] = { 0 };
    while (file)
    {
        file >> command;
        if (wcscmp(command, L"#") == 0)
        {
            file.ignore(1024, L'\n');
        }
        else if (wcscmp(command, L"map_Kd") == 0)
        {
            file.ignore();
            wchar_t map_Kd[256];
            file >> map_Kd;
            ResourceManager::CreateFilenameToRefer(map_Kd, objfilename, map_Kd, false);
            mMaterials.rbegin()->map_Kd = map_Kd;
            file.ignore(1024, L'\n');
        }
        else if (wcscmp(command, L"newmtl") == 0)
        {
            file.ignore();
            wchar_t newmtl[256];
            file >> newmtl;
            
            mMaterials.emplace_back(MaterialData());
            mMaterials.back().newmtl = newmtl;
        }
        else if (wcscmp(command, L"Ka") == 0)
        {
            float r, g, b;
            file >> r >> g >> b;
            mMaterials.rbegin()->Ka = DirectX::XMFLOAT3(r, g, b);
            file.ignore(1024, L'\n');
        }
        else if (wcscmp(command, L"Kd") == 0)
        {
            float r, g, b;
            file >> r >> g >> b;
            mMaterials.rbegin()->Kd = DirectX::XMFLOAT3(r, g, b);
            file.ignore(1024, L'\n');
        }
        else if (wcscmp(command, L"Ks") == 0)
        {
            float r, g, b;
            file >> r >> g >> b;
            mMaterials.rbegin()->Ks = DirectX::XMFLOAT3(r, g, b);
            file.ignore(1024, L'\n');
        }
        else if (wcscmp(command, L"illum") == 0)
        {
            u_int illum;
            file >> illum;
            mMaterials.rbegin()->illum = illum;
            file.ignore(1024, L'\n');
        }
        else
        {
            file.ignore(1024, L'\n');
        }
    }
    file.close();
}
