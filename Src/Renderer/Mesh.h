#pragma once
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "D3D_Helper.h"
#include "Shader.h"
#include "GPUBuffer.h"
#include "GraphicsDevice.h"
#include "./Engine/DataStructures.h"
#include "./Engine/CameraController.h"

struct MeshSubset
{
	uint32_t indexStart = 0;
	uint32_t indexCount = 0;
	uint32_t materialId = 0;

	MeshSubset() = default;
	~MeshSubset() = default;
	MeshSubset(const MeshSubset& subset)
	{
		if (this != &subset)
		{
			indexStart = subset.indexStart;
			indexCount = subset.indexCount;
			materialId = subset.materialId;
		}
	}
	MeshSubset(MeshSubset&& subset) noexcept
	{
		if (this != &subset)
		{
			indexStart = subset.indexStart;
			indexCount = subset.indexCount;
			materialId = subset.materialId;
		}
	}
	MeshSubset& operator=(const MeshSubset& subset)
	{
		if (this != &subset)
		{
			indexStart = subset.indexStart;
			indexCount = subset.indexCount;
			materialId = subset.materialId;
		}
		return *this;
	}
	MeshSubset& operator=(MeshSubset&& subset) noexcept
	{
		if (this != &subset)
		{
			indexStart = subset.indexStart;
			indexCount = subset.indexCount;
			materialId = subset.materialId;
		}
		return *this;
	}
};

struct VertexData
{
	XMFLOAT3 position = {};
	XMFLOAT3 normal = {};
	XMFLOAT2 texcoord = {};
	XMFLOAT4 color = {1,1,1,1};
	XMFLOAT3 tangent = {};
	XMFLOAT3 binormal = {};

	VertexData() = default;
	~VertexData() = default;
	VertexData(const VertexData& vertex)
	{
		if (this != &vertex)
		{
			position = vertex.position;
			normal = vertex.normal;
			texcoord = vertex.texcoord;
			color = vertex.color;
			tangent = vertex.tangent;
			binormal = vertex.binormal;
		}
	}
	VertexData(VertexData&& vertex) noexcept
	{
		if (this != &vertex)
		{
			position = std::move(vertex.position);
			normal = std::move(vertex.normal);
			color = std::move(vertex.color);
			texcoord = std::move(vertex.texcoord);
			tangent = std::move(vertex.tangent);
			binormal = std::move(vertex.binormal);
		}
	}
	VertexData& operator=(const VertexData& vertex)
	{
		if (this != &vertex)
		{
			position = vertex.position;
			normal = vertex.normal;
			texcoord = vertex.texcoord;
			color = vertex.color;
			tangent = vertex.tangent;
			binormal = vertex.binormal;
		}
		return *this;
	}
	VertexData& operator=(VertexData&& vertex) noexcept
	{
		if (this != &vertex)
		{
			position = std::move(vertex.position);
			normal = std::move(vertex.normal);
			texcoord = std::move(vertex.texcoord);
			color = std::move(vertex.color);
			tangent = std::move(vertex.tangent);
			binormal = std::move(vertex.binormal);
		}
		return *this;
	}
};

static constexpr uint32_t BONE_INFLUENCE_SIZE_MAX = 4;
struct Vertex_BoneData
{
	uint32_t index[BONE_INFLUENCE_SIZE_MAX] = { 0,0,0,0 };
	float    weight[BONE_INFLUENCE_SIZE_MAX] = { 0,0,0,0 };
	uint32_t count = 0;

	Vertex_BoneData() = default;
	~Vertex_BoneData() = default;
	Vertex_BoneData(const Vertex_BoneData& vertex)
	{
		if (this == &vertex)
		{
			for (size_t i = 0; i < BONE_INFLUENCE_SIZE_MAX; ++i)
			{
				index[i] = vertex.index[i];
				weight[i] = vertex.weight[i];
			}
		}
	}
	Vertex_BoneData(Vertex_BoneData&& vertex) noexcept
	{
		if (this == &vertex)
		{
			for (size_t i = 0; i < BONE_INFLUENCE_SIZE_MAX; ++i)
			{
				index[i] = vertex.index[i];
				weight[i] = vertex.weight[i];
			}
		}
	}
	Vertex_BoneData& operator=(const Vertex_BoneData& vertex)
	{
		if (this == &vertex)
		{
			for (size_t i = 0; i < BONE_INFLUENCE_SIZE_MAX; ++i)
			{
				index[i] = vertex.index[i];
				weight[i] = vertex.weight[i];
			}
		}
		return *this;
	}
	Vertex_BoneData& operator=(Vertex_BoneData&& vertex) noexcept
	{
		if (this == &vertex)
		{
			for (size_t i = 0; i < BONE_INFLUENCE_SIZE_MAX; ++i)
			{
				index[i] = vertex.index[i];
				weight[i] = vertex.weight[i];
			}
		}
		return *this;
	}
};

struct MeshData
{
	std::vector<VertexData> vertices;
	std::vector<Vertex_BoneData> verticesBone;
	std::vector<uint32_t>   indices;
	std::vector<MeshSubset> subsets;

	MeshData() = default;
	~MeshData() = default;
	MeshData(const MeshData& mesh)
	{
		if (this == &mesh)
		{
			vertices = mesh.vertices;
			verticesBone = mesh.verticesBone;
			indices = mesh.indices;
			subsets = mesh.subsets;
		}
	}
	MeshData(MeshData&& mesh) noexcept
	{
		if (this == &mesh)
		{
			vertices = std::move(mesh.vertices);
			verticesBone = std::move(mesh.verticesBone);
			indices = std::move(mesh.indices);
			subsets = std::move(mesh.subsets);
		}
	}
	MeshData& operator=(const MeshData& mesh)
	{
		if (this == &mesh)
		{
			vertices = mesh.vertices;
			verticesBone = mesh.verticesBone;
			indices = mesh.indices;
			subsets = mesh.subsets;
		}
		return *this;
	}
	MeshData& operator=(MeshData&& mesh) noexcept
	{
		if (this == &mesh)
		{
			vertices = std::move(mesh.vertices);
			verticesBone = std::move(mesh.verticesBone);
			indices = std::move(mesh.indices);
			subsets = std::move(mesh.subsets);
		}
		return *this;
	}
};

class Mesh
{
public:
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT3 tangent;
        DirectX::XMFLOAT3 binormal;
    };

    struct CBufferForMesh
    {
        DirectX::XMFLOAT4X4 WVP;   // world * view * projection matrix
        DirectX::XMFLOAT4X4 world;           // world transformation matrix
        DirectX::XMFLOAT4X4 invViewProj;
        DirectX::XMFLOAT4X4 invView;
        DirectX::XMFLOAT4X4 invProj;
        DirectX::XMFLOAT4 param = {};
    };
    struct CBufferForMaterial
    {
        DirectX::XMFLOAT4 mat_color = { 1, 1, 1, 1 };
        DirectX::XMFLOAT3 specularColor;
        float metalness;
        float roughness;
        float diffuse = 0.0f;
        float specular = 0.0f;
        int   textureConfig;
    };

protected:
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;

    
protected:
    Microsoft::WRL::ComPtr<ID3D11Buffer>            mpConstantBufferMesh = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            mpConstantBufferMaterial = nullptr;

    // RenderState
    //Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_pRasterizerWire = nullptr;
    //Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_pRasterizerSolid = nullptr;
    //D3D11_PRIMITIVE_TOPOLOGY                        mTopologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    Graphics::PrimitiveTopologyID                   mToplogyID = Graphics::TRIANGLELIST;
public:

    Mesh() {}
    virtual ~Mesh() {}

    virtual void CreateBuffers(Graphics::GraphicsDevice* p_device) = 0;

    virtual void Render(
        Graphics::GraphicsDevice* p_device,
        float elapse_time,
        const DirectX::XMMATRIX& world,
        CameraController* camera,
        Shader* shader,
        const Material& mat_data,
        bool isShadow = false,
        bool isSolid = true
    ) = 0;

    //inline void SetTopology(D3D11_PRIMITIVE_TOPOLOGY type) { mTopologyType = type; }
};