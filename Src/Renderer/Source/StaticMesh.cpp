#include "StaticMesh.h"

#include <fstream>

#include "Texture.h"
#include "ResourceManager.h"
#include "Shader.h"

//#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/Light.h"

#include "./Utilities/misc.h"

using namespace DirectX;

StaticMesh::StaticMesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t* OBJFile, bool isFlippingV):Mesh()
{
    HRESULT hr = S_OK;


    LoadOBJFile(device, OBJFile, isFlippingV);

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



    ////--> Set InputLayouts
    //D3D11_INPUT_ELEMENT_DESC inputLayouts[] = {
    //    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //};

    //ResourceManager::CreateVSFromFile(device, "./Data/Shaders/phong_obj_vs.cso", mVertexShader.GetAddressOf(), mInputLayout.GetAddressOf(), inputLayouts, ARRAYSIZE(inputLayouts));
    //ResourceManager::CreatePSFromFile(device, "./Data/Shaders/phong_ps.cso", mPixelShader.GetAddressOf());

    for (auto &material : mMaterials)
    {
        material.texture = std::make_shared<Texture>();
        if (material.map_Kd == L"\0")
        {
            material.texture->Load(device);
            continue;
        }
        material.texture->Load(device, material.map_Kd.data());
    }



    //-->Create RasterizerState
    hr = device->CreateRasterizerState(
        &CD3D11_RASTERIZER_DESC(
            D3D11_FILL_WIREFRAME,     // D3D11_FILL_MODE FillMode;
            D3D11_CULL_BACK,          // D3D11_CULL_MODE CullMode;
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
            D3D11_CULL_BACK,          // D3D11_CULL_MODE CullMode;
            FALSE,                    // BOOL FrontCounterClockwise;
            0,                        // INT DepthBias;
            0.0f,                     // FLOAT DepthBiasClamp;
            0.0f,                     // FLOAT SlopeScaledDepthBias;
            TRUE,                     // BOOL DepthClipEnable;
            FALSE,                    // BOOL ScissorEnable;
            FALSE,                    // BOOL MultisampleEnable;
            FALSE                     // BOOL AntialiasedLineEnable;
            ),
        m_pRasterizerSolid.GetAddressOf()
        );
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));





}

StaticMesh::~StaticMesh()
{
}


void StaticMesh::CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device)
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

void StaticMesh::Render(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
    float elapsed_time,
    const DirectX::XMMATRIX& world,
    const std::shared_ptr<CameraController>& camera,
    const std::shared_ptr<Shader>& shader,
    const DirectX::XMFLOAT4& mat_color,
    bool isShadow,
    bool isSolid
)
{

    HRESULT hr = S_OK;

    shader->activateShaders(imm_context);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    imm_context->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
    imm_context->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    imm_context->IASetPrimitiveTopology(mTopologyType);
    //imm_context->IASetInputLayout(mInputLayout.Get());
    isSolid
        ? imm_context->RSSetState(m_pRasterizerSolid.Get())
        : imm_context->RSSetState(m_pRasterizerWire.Get());
    //imm_context->VSSetShader(mVertexShader.Get(), NULL, 0);
    //imm_context->RSSetViewports(numViewport, &viewport);
    //imm_context->PSSetShader(mPixelShader.Get(), NULL, 0);

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

    for (auto &material : mMaterials)
    {
        CBufferForMesh meshbuffer = {};
        meshbuffer.m_WVP = WVP;
        meshbuffer.m_world = W;
        meshbuffer.m_mat_color.x = mat_color.x * material.Kd.x;
        meshbuffer.m_mat_color.y = mat_color.y * material.Kd.y;
        meshbuffer.m_mat_color.z = mat_color.z * material.Kd.z;
        meshbuffer.m_mat_color.w = mat_color.w;

        imm_context->UpdateSubresource(m_pConstantBufferMesh.Get(), 0, nullptr, &meshbuffer, 0, 0);

        imm_context->VSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
        imm_context->HSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
        imm_context->DSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
        imm_context->GSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());

        imm_context->PSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());

        material.texture->Set(imm_context);

        for (auto &subset : mSubsets)
        {
            //if (material.newmtl == subset.usemtl)
            //{
                imm_context->DrawIndexed(subset.index_count, subset.index_start, 0);
            //}
        }
    }


}

void StaticMesh::LoadOBJFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t *filename, bool isFlippingV)
{
    //std::vector<Vertex> vertices;
    //std::vector<WORD> indices;
    u_int currentIndex = 0;

    std::vector<XMFLOAT3> positions;
    std::vector<XMFLOAT3> normals;
    std::vector<XMFLOAT2> texcoords;

    std::vector<std::wstring> mtl_filenames;

    std::wifstream file(filename);
    _ASSERT_EXPR_A(file, L"OBJ file was not found.");

    wchar_t command[256];
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
            for (auto i = 0; i < 3; ++i)
            {
                Vertex vertex;
                u_int v, vt, vn;

                file >> v;
                vertex.position = positions[v - 1];
                if (L'/' == file.peek())
                {
                    file.ignore();
                    if (L'/' != file.peek())
                    {
                        file >> vt;
                        vertex.texcoord = texcoords[vt - 1];
                    }
                    if (L'/' == file.peek())
                    {
                        file.ignore();
                        file >> vn;
                        vertex.normal = normals[vn - 1];
                    }
                    vertex.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                }
                mVertices.emplace_back(vertex);
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
        //else if (0 == wcscmp(command, L"g"))
        //{
        //    wchar_t usemtl[MAX_PATH] = { 0 };
        //    file >> usemtl;

        //    subset current_subset = {};
        //    current_subset.usemtl = usemtl;
        //    current_subset.index_start = indices.size();
        //    mSubsets.push_back(current_subset);
        //}
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


    LoadMTLFile(device, filename, mtl_filenames);

    CreateBuffers(device/*, vertices.data(), indices.data(), vertices.size(), indices.size()*/);
}

void StaticMesh::LoadMTLFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t *objfilename, std::vector<std::wstring>& mtlFilenames)
{
    wchar_t mtlFilename[256];

    ResourceManager::CreateFilenameToRefer(mtlFilename, objfilename, mtlFilenames[0].c_str());

    std::wifstream file(mtlFilename);
    /*_ASSERT_EXPR(file, L"MTL file doesn't exist.");*/

    if (!file.is_open())
    {
        for (auto& subset : mSubsets)
        {
            Material material;
            material.map_Kd = L"\0";
            mMaterials.emplace_back(material);
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
            ResourceManager::CreateFilenameToRefer(map_Kd, objfilename, map_Kd);
            mMaterials.rbegin()->map_Kd = map_Kd;
            file.ignore(1024, L'\n');
        }
        else if (wcscmp(command, L"newmtl") == 0)
        {
            file.ignore();
            wchar_t newmtl[256];
            Material material;
            file >> newmtl;
            material.newmtl = newmtl;
            mMaterials.emplace_back(material);
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
