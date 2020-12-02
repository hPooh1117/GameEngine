#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl\client.h>
#include <vector>
#include <fbxsdk.h>
#include <unordered_map>
#include <map>
#include <memory>
#include "Texture.h"

namespace FbxInfo
{
	static constexpr unsigned int MAX_INFLUENCES = 4;
	static constexpr unsigned int MAX_BONES = 150;
	static constexpr unsigned int MAX_MOTIONS = 10;
	static constexpr float        SAMPLE_TIME = 1.0f / 60.0f;

	struct Vertex
	{
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT3 normal = {};
		DirectX::XMFLOAT2 texcoord = {};
		DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
		FLOAT bone_weights[MAX_INFLUENCES] = { 1, 0, 0, 0 };
		INT   bone_indices[MAX_INFLUENCES] = {};
		DirectX::XMFLOAT3 tangent = {};
		DirectX::XMFLOAT3 binormal = {};

		Vertex() = default;
		Vertex(const Vertex&) = default;
		Vertex& operator=(const Vertex&) = default;
	};

	struct Material
	{
		DirectX::XMFLOAT4 color = {1.0f, 1.0f, 1.0f, 1.0f};
		std::shared_ptr<Texture> texture;
		std::wstring texture_filename;

		Material() = default;
		Material(const Material&) = default;
		Material& operator=(const Material&) = default;
	};

	struct Texture
	{
		std::unique_ptr<Texture> p_texture;
		const wchar_t* filename;
	};

	struct Subset
	{
		u_int indexStart = 0;
		u_int indexCount = 0;
		Material diffuse = {};

		Subset() = default;
		Subset(const Subset& other) = default;
		Subset& operator=(const Subset& other) = default;
	};


	struct BoneData
	{
		DirectX::XMFLOAT4X4 transform = {};
		std::string name;
		DirectX::XMFLOAT4X4 offset = {};

		BoneData() = default;
		BoneData(const BoneData&) = default;
		BoneData& operator=(const BoneData&) = default;
	};

	struct Motion
	{


		unsigned int   frameSize = 0;
		float          animeTick = 0.0f;
		bool           bIsLooped = true;
		std::vector<std::vector<DirectX::XMFLOAT4X4>> keys;

		Motion() = default;
		Motion(const Motion&) = default;
		Motion& operator=(const Motion&) = default;
	};

#pragma region OldImpl
	//struct Bone
//{
//	DirectX::XMFLOAT4X4 transform = {};
//	std::string name;
//	Bone() = default;
//	Bone(const Bone&) = default;
//	Bone& operator=(const Bone&) = default;
//};

	//struct Weight
	//{
	//	int count;
	//	int bone[4];
	//	float weight[4];
	//};

	//typedef std::vector<Bone> Skeltal;
	//struct SkeltalAnimation : public std::vector<Skeltal>
	//{
	//	float sampling_time = 1 / 24.0f;
	//	float animation_tick = 0.0f;
	//	int number_of_frame;
	//	SkeltalAnimation() = default;
	//	SkeltalAnimation(const SkeltalAnimation&) = default;
	//	SkeltalAnimation& operator=(const SkeltalAnimation&) = default;
	//};
#pragma endregion
}

struct MyFbxMesh
{
public:
	std::vector<FbxInfo::Vertex> mVertices;
	std::vector<u_int>           mIndices;
	std::vector<FbxInfo::Subset> mSubsets;

	DirectX::XMFLOAT4X4 mGlobalTransform = { 
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 
	};

	std::vector<FbxInfo::BoneData> m_bone_data;

	std::map<std::string, FbxInfo::Motion> mMotions;

	//FbxInfo::SkeltalAnimation m_skeltal_animation;

	int m_start_frame = 0;


	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer  = nullptr;

	MyFbxMesh() = default;
	MyFbxMesh(const MyFbxMesh& other) = default;
	MyFbxMesh& operator=(const MyFbxMesh& other) = default;
	
};