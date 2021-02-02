#pragma once
#include "MeshLoader.h"

#include <fbxsdk.h>
#include <map>

class FbxSdkLoader : public MeshLoader
{
private:
	std::vector<BoneData> mBoneTable;
	uint32_t mBoneCount = 0;

	MotionData motionData;

	std::vector<Vertex_BoneData> mVerticesBoneTable;

public:
	FbxSdkLoader() = default;
	virtual ~FbxSdkLoader() = default;
	virtual bool Load(const wchar_t* file_path) override;
	
private:
	void LoadMotion(FbxScene* scene);
	void LoadVertices(FbxMesh* fbxmesh, MeshData& mesh);
	void LoadMeshAnimation(FbxMesh* fbxmesh, MeshData& mesh);
	void LoadSkins(FbxMesh* fbxmesh, BoneData& bone, MeshData& mesh);
	int  FindBone(const char* name);
	void LoadKeyFrames(std::string name, int bone, FbxNode* bone_node);
	void SetUpSubsets(FbxMesh* fbxmesh, MeshData& mesh);
public:
};

