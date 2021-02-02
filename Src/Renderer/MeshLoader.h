#pragma once
#include "./Utilities/CommonInclude.h"
#include "Mesh.h"
#include <vector>


struct MaterialData
{
	XMFLOAT4 color;
	std::wstring textureFilePath = L"";
};

struct BoneData
{
	XMFLOAT4X4  transform = {};
	XMFLOAT4X4  offset = {};
	std::string name = "";
};

struct MotionData
{
	uint32_t       frameCount = 0;
	int            startFrame = 0;
	float          animeTick = 0.0f;
	bool           isLooped = true;
	std::vector<std::vector<DirectX::XMFLOAT4X4>> keys;
};

class MeshLoader
{
protected:
	uint32_t mMeshCount = 0; // 合計メッシュ数
	std::vector<MeshData> mMeshes; // メッシュデータ一時格納用

	
	uint32_t mVertexCount = 0; // 合計頂点数
	uint32_t mIndexCount = 0; // 合計インデックス数

	uint32_t mMaterialCount = 0; // 合計マテリアル数
	std::vector<MaterialData> mMaterials; // マテリアルデータ一時格納用

public:
	MeshLoader() {}
	virtual ~MeshLoader() {}

	virtual bool Load(const wchar_t* file_path) = 0;
	inline void GetMeshData(std::vector<MeshData>& meshContainer)
	{
		meshContainer = std::move(mMeshes);
	}
	inline void GetMaterialData(std::vector<MaterialData>& materialContainer)
	{
		materialContainer = std::move(mMaterials);
	}
};

