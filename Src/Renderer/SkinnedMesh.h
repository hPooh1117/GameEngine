#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl\client.h>
#include <vector>
#include <memory>
#include <fbxsdk.h> 
#include "FbxMeshInfo.h"
#include "Mesh.h"
#include "./Utilities/Util.h"

class Shader;

class SkinnedMesh : public Mesh
{
public:
	DirectX::XMFLOAT4X4 m_coordinate_conversion = {
	1, 0, 0, 0,
	0, 0, 1, 0,
	0, 1, 0, 0,
	0, 0, 0, 1
	};

	static constexpr unsigned int MAX_MOTION_INTERPOLATION = 60;

private:
	struct CBufferForBone
	{
		DirectX::XMFLOAT4X4 m_bone_transforms[FbxInfo::MAX_BONES];
		//DirectX::XMFLOAT4X4 m_motion_transforms[FbxInfo::MAX_BONES];
		//DirectX::XMFLOAT4X4 m_bone_offset[FbxInfo::MAX_BONES];
	};


	std::vector<MyFbxMesh> m_meshes;
	std::string mCurrentMotion = "default";
	std::string mPreviousMotion = "default";
	FrameTimer mFrameInterpolation;

	// convert coordinate system from 'UP: +Z FRONT: +Y RIGHT-HAND' to
	// 'UP: +Y FRONT: +Z LEFT-HAND'

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constant_buffer_bone;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>      m_sampler_state;
	D3D11_TEXTURE2D_DESC m_texture2D_desc;

public:
	SkinnedMesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, const char* filename);
	~SkinnedMesh() = default;


public:
	void LoadFbxFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const char* filename);

	virtual void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;

	virtual void Render(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
		float elapsed_time,
		const DirectX::XMMATRIX& world,
		const std::shared_ptr<CameraController>& camera,
		const std::shared_ptr<Shader>& shader,
		const DirectX::XMFLOAT4& mat_color = DirectX::XMFLOAT4( 1, 1, 1, 1 ),
		bool isShadow = false,
		bool isSolid = true
	) override;

	bool AddMotion(std::string& name, const char* filename);

	DirectX::XMMATRIX Lerp(DirectX::XMFLOAT4X4& A, DirectX::XMFLOAT4X4& B, float rate);

	void Play(std::string name, bool isLooped = false);
	DirectX::XMMATRIX CalculateMotionMatrix(int frame, int bone_id, MyFbxMesh& mesh);
};

