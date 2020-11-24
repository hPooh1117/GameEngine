#pragma once
#include "FbxMeshInfo.h"
#include "Mesh.h"
#include "./Utilities/Util.h"


class SkinnedMesh : public Mesh
{
public:
	enum CoordSystem
	{
		EY_UP_LHS,
		EZ_UP_LHS,
		EY_UP_RHS,
	};

	static constexpr unsigned int CG_SOFTWARE_TYPE = 3;
	static const DirectX::XMFLOAT4X4 COORD_CONVERSION[CG_SOFTWARE_TYPE];



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
	unsigned int mCoordSystem;

	// convert coordinate system from 'UP: +Z FRONT: +Y RIGHT-HAND' to
	// 'UP: +Y FRONT: +Z LEFT-HAND'

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constant_buffer_bone;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>      m_sampler_state;
	D3D11_TEXTURE2D_DESC m_texture2D_desc;

public:
	SkinnedMesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, const char* filename, unsigned int coord_system);
	~SkinnedMesh() = default;


public:
	void LoadFbxFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const char* filename);

	virtual void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device) override;

	virtual void Render(
		D3D::DeviceContextPtr& imm_context,
		float elapsed_time,
		const DirectX::XMMATRIX& world,
		CameraController* camera,
		Shader* shader,
		const MaterialData& mat_data,
		bool isShadow = false,
		bool isSolid = true
	) override;

	bool AddMotion(std::string& name, const char* filename);

	DirectX::XMMATRIX Lerp(DirectX::XMFLOAT4X4& A, DirectX::XMFLOAT4X4& B, float rate);

	void Play(std::string name, int blend_time, bool isLooped = false);
	DirectX::XMMATRIX CalculateMotionMatrix(int frame, int bone_id, MyFbxMesh& mesh);

};

