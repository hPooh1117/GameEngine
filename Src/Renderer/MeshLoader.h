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
	uint32_t mMeshCount = 0; // ���v���b�V����
	std::vector<MeshData> mMeshes; // ���b�V���f�[�^�ꎞ�i�[�p

	
	uint32_t mVertexCount = 0; // ���v���_��
	uint32_t mIndexCount = 0; // ���v�C���f�b�N�X��

	uint32_t mMaterialCount = 0; // ���v�}�e���A����
	std::vector<MaterialData> mMaterials; // �}�e���A���f�[�^�ꎞ�i�[�p

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

