#include "SkinnedMesh.h"

#include <functional>  
#include <chrono>
#include <iostream>

#include "FbxMeshInfo.h"
#include "FbxLoader.h"
#include "ResourceManager.h"
#include "Shader.h"

#include "./Engine/MainCamera.h"
#include "./Engine/CameraController.h"
#include "./Engine/Light.h"
#include "./Engine/GameSystem.h"

#include "./Utilities/misc.h"
#include "./Utilities/Log.h"

//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;
using namespace fbxsdk;

//----------------------------------------------------------------------------------------------------------------------------
const DirectX::XMFLOAT4X4 SkinnedMesh::COORD_CONVERSION[CG_SOFTWARE_TYPE] =
{
	// DEFAULT(LHS +Y-UP)
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	},

	// UNREAL ENGINE(LHS +Z-UP)
	{
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
	},
	// MAYA(RHS +Y-UP)
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0,-1, 0,
		0, 0, 0, 1
	},
};

SkinnedMesh::SkinnedMesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, const char* filename, unsigned int coord_system)
	:Mesh(), 
	mFrameInterpolation(MAX_MOTION_INTERPOLATION),
	mCoordSystem(coord_system)
{

	LoadFbxFile(device, filename);

	mFrameInterpolation.SetTime(MAX_MOTION_INTERPOLATION);


	HRESULT hr = S_OK;


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


	hr = device->CreateBuffer(
		&CD3D11_BUFFER_DESC(
			sizeof(CBufferForBone),
			D3D11_BIND_CONSTANT_BUFFER,
			D3D11_USAGE_DEFAULT,
			0, 0, 0
		),
		nullptr,
		m_constant_buffer_bone.GetAddressOf()
	);

	hr = device->CreateBuffer(
		&CD3D11_BUFFER_DESC(
			sizeof(CBufferForMaterial),
			D3D11_BIND_CONSTANT_BUFFER,
			D3D11_USAGE_DEFAULT,
			0,
			0,
			0
		),
		nullptr,
		m_pConstantBufferMaterial.GetAddressOf()
	);
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));






	//-->Create RasterizerState

	hr = device->CreateRasterizerState(
		&CD3D11_RASTERIZER_DESC(
			D3D11_FILL_SOLID,                    // D3D11_FILL_MODE FillMode;
			D3D11_CULL_BACK,                     // D3D11_CULL_MODE CullMode;
			mCoordSystem > 0 ? TRUE : FALSE,     // BOOL FrontCounterClockwise;
			0,                                   // INT DepthBias;
			0.0f,                                // FLOAT DepthBiasClamp;
			0.0f,                                // FLOAT SlopeScaledDepthBias;
			TRUE,                                // BOOL DepthClipEnable;
			FALSE,                               // BOOL ScissorEnable;
			FALSE,                               // BOOL MultisampleEnable;
			FALSE                                // BOOL AntialiasedLineEnable;
		),
		m_pRasterizerSolid.GetAddressOf()
	);
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));



	hr = device->CreateRasterizerState(
		&CD3D11_RASTERIZER_DESC(
			D3D11_FILL_WIREFRAME,         // D3D11_FILL_MODE FillMode;
			D3D11_CULL_BACK,          // D3D11_CULL_MODE CullMode;
			TRUE,                     // BOOL FrontCounterClockwise;
			0,                        // INT DepthBias;
			0.0f,                     // FLOAT DepthBiasClamp;
			0.0f,                     // FLOAT SlopeScaledDepthBias;
			TRUE,                     // BOOL DepthClipEnable;
			FALSE,                    // BOOL ScissorEnable;
			FALSE,                    // BOOL MultisampleEnable;
			FALSE                     // BOOL AntialiasedLineEnable;
		),
		m_pRasterizerWire.GetAddressOf()
	);
	_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));


}

//----------------------------------------------------------------------------------------------------------------------------

void SkinnedMesh::LoadFbxFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const char* filename)
{
	std::unique_ptr<FbxLoader> fbxLoader = std::make_unique<FbxLoader>();
	std::chrono::system_clock::time_point start, end;
	start = std::chrono::system_clock::now();

	if (fbxLoader->LoadFbxFile(device, filename, m_meshes))
	{
		CreateBuffers(device);
	}
	end = std::chrono::system_clock::now();

#ifdef _DEBUG
	double elapsed = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

	Log::Info("[SKINNED MESH] Loaded FBX Mesh. Filename: %s (%.2f ms)", filename, elapsed);
#endif
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinnedMesh::CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	for (auto& mesh : m_meshes)
	{
		HRESULT hr = S_OK;

		//--> Create Vertex Buffer
		D3D11_SUBRESOURCE_DATA vSrData = {};
		vSrData.pSysMem = mesh.mVertices.data();
		vSrData.SysMemPitch = 0;
		vSrData.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(
			&CD3D11_BUFFER_DESC(
				sizeof(FbxInfo::Vertex) * mesh.mVertices.size(),
				D3D11_BIND_VERTEX_BUFFER,
				D3D11_USAGE_IMMUTABLE,
				0,
				0,
				0
			),
			&vSrData,
			mesh.m_pVertexBuffer.GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));



		D3D11_SUBRESOURCE_DATA iSrData = {};
		iSrData.pSysMem = mesh.mIndices.data();
		iSrData.SysMemPitch = 0;
		iSrData.SysMemSlicePitch = 0;


		hr = device->CreateBuffer(
			&CD3D11_BUFFER_DESC(
				sizeof(u_int) * mesh.mIndices.size(),
				D3D11_BIND_INDEX_BUFFER,
				D3D11_USAGE_IMMUTABLE,
				0,
				0,
				0
			),
			&iSrData,
			mesh.m_pIndexBuffer.GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	}
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinnedMesh::Render(
	D3D::DeviceContextPtr& imm_context,
	float elapsed_time,
	const DirectX::XMMATRIX& world,
	CameraController* camera,
	Shader* shader,
	const MaterialData& mat_data,
	bool isShadow,
	bool isSolid
)
{
	HRESULT hr = S_OK;
	if (shader != nullptr)
	{
		shader->activateShaders(imm_context);
	}

	XMFLOAT4X4 W, WVP;
	XMStoreFloat4x4(&W, world);
	XMStoreFloat4x4(&WVP, world * (isShadow ? camera->GetOrthoView() * camera->GetOrthoProj(imm_context) : camera->GetViewMatrix() * camera->GetProjMatrix(imm_context)));


	for (auto& mesh : m_meshes)
	{
		CBufferForMesh meshData = {};
		CBufferForBone boneData = {};

		UINT stride = sizeof(FbxInfo::Vertex);
		UINT offset = 0;




		if (mesh.m_motions[mCurrentMotion].frameSize > 0)
		{
			FbxInfo::Motion& motion = mesh.m_motions[mCurrentMotion];
			int frame = static_cast<int>(motion.animeTick / motion.SAMPLE_TIME);
			if (frame > motion.frameSize - 1)
			{
				frame = 0;
				motion.animeTick = 0;
			}
			size_t numberOfBones = mesh.m_bone_data.size();
			_ASSERT_EXPR(numberOfBones < FbxInfo::MAX_BONES, L"'numberOfBones' exceeds MAX_BONES.");
			for (size_t i = 0; i < numberOfBones; ++i)
			{
				DirectX::XMStoreFloat4x4(
					&boneData.m_bone_transforms[i],
					XMLoadFloat4x4(&mesh.m_bone_data.at(i).offset) *
					CalculateMotionMatrix(frame, i, mesh)
				);
				//boneData.m_bone_offset[i] = mesh.m_bone_data.at(i).offset;
				//boneData.m_motion_transforms[i] = CalculateMotionMatrix(frame, i, mesh);
			}
			motion.animeTick += elapsed_time * 0.5f;
		}
		else
		{

			size_t numberOfBones = mesh.m_bone_data.size();
			for (size_t i = 0; i < numberOfBones; ++i)
			{
				DirectX::XMStoreFloat4x4(
					&boneData.m_bone_transforms[i],
					DirectX::XMMatrixIdentity()
				);
				//XMStoreFloat4x4(&boneData.m_bone_offset[i], XMMatrixIdentity());
				//XMStoreFloat4x4(&boneData.m_motion_transforms[i], XMMatrixIdentity());

			}
		}
#pragma region CPU_Skinning
		//for (auto v = 0u;  v < mesh.mVertices.size(); ++v)
		//{
		//	Vector3 p = mesh.mVertices[v].position;
		//	Vector3 n = mesh.mVertices[v].normal;


		//	mesh.mVertices[v].position.x = 0;
		//	mesh.mVertices[v].position.y = 0;
		//	mesh.mVertices[v].position.z = 0;

		//	mesh.mVertices[v].normal.x = 0;
		//	mesh.mVertices[v].normal.y = 0;
		//	mesh.mVertices[v].normal.z = 0;

		//	for (auto i = 0u; i < 4; ++i)
		//	{
		//		const DirectX::XMFLOAT4X4& key = boneData.m_bone_transforms[mesh.mVertices[v].bone_indices[i]];
		//		mesh.mVertices[v].position.x += mesh.mVertices[v].bone_weights[i] * (p.x * key._11 + p.y * key._21 + p.z * key._31 + 1 * key._41);
		//		mesh.mVertices[v].position.y += mesh.mVertices[v].bone_weights[i] * (p.x * key._12 + p.y * key._22 + p.z * key._32 + 1 * key._42);
		//		mesh.mVertices[v].position.z += mesh.mVertices[v].bone_weights[i] * (p.x * key._13 + p.y * key._23 + p.z * key._33 + 1 * key._43);

		//		mesh.mVertices[v].normal.x += mesh.mVertices[v].bone_weights[i] * (n.x * key._11 + n.y * key._21 + n.z * key._31);
		//		mesh.mVertices[v].normal.y += mesh.mVertices[v].bone_weights[i] * (n.x * key._12 + n.y * key._22 + n.z * key._32);
		//		mesh.mVertices[v].normal.z += mesh.mVertices[v].bone_weights[i] * (n.x * key._13 + n.y * key._23 + n.z * key._33);
		//	}
		//}
		//imm_context->UpdateSubresource(mesh.mpVertexBuffer.Get(), 0, NULL, mesh.mVertices.data(), 0, 0);
#pragma endregion

		imm_context->IASetVertexBuffers(0, 1, mesh.m_pVertexBuffer.GetAddressOf(), &stride, &offset);
		imm_context->IASetIndexBuffer(mesh.m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		imm_context->IASetPrimitiveTopology(mTopologyType);
		imm_context->RSSetState(isSolid ? m_pRasterizerSolid.Get() : m_pRasterizerWire.Get());

		DirectX::XMStoreFloat4x4(
			&meshData.WVP,
			DirectX::XMLoadFloat4x4(&mesh.mGlobalTransform) *
			DirectX::XMLoadFloat4x4(&COORD_CONVERSION[mCoordSystem]) *
			DirectX::XMLoadFloat4x4(&WVP)
		);
		DirectX::XMStoreFloat4x4(
			&meshData.world,
			DirectX::XMLoadFloat4x4(&mesh.mGlobalTransform) *
			DirectX::XMLoadFloat4x4(&COORD_CONVERSION[mCoordSystem]) *
			DirectX::XMLoadFloat4x4(&W));
		XMStoreFloat4x4(&meshData.invProj, camera->GetInvProjViewMatrix(imm_context));

		imm_context->UpdateSubresource(m_pConstantBufferMesh.Get(), 0, nullptr, &meshData, 0, 0);
		imm_context->VSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
		imm_context->HSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
		imm_context->DSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
		imm_context->GSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());
		imm_context->PSSetConstantBuffers(0, 1, m_pConstantBufferMesh.GetAddressOf());

		imm_context->UpdateSubresource(m_constant_buffer_bone.Get(), 0, nullptr, &boneData, 0, 0);
		imm_context->VSSetConstantBuffers(4, 1, m_constant_buffer_bone.GetAddressOf());
		imm_context->PSSetConstantBuffers(4, 1, m_constant_buffer_bone.GetAddressOf());

		for (FbxInfo::Subset& subset : mesh.mSubsets)
		{
			CBufferForMaterial matData = {};
			matData.mat_color.x = subset.diffuse.color.x * mat_data.mat_color.x;
			matData.mat_color.y = subset.diffuse.color.y * mat_data.mat_color.y;
			matData.mat_color.z = subset.diffuse.color.z * mat_data.mat_color.z;
			matData.mat_color.w = mat_data.mat_color.w;

			matData.metalness = mat_data.metalness;
			matData.roughness = mat_data.roughness;
			matData.specularColor = mat_data.specularColor;
			matData.brdfFactor = mat_data.brdfFactor;

			imm_context->UpdateSubresource(m_pConstantBufferMaterial.Get(), 0, nullptr, &matData, 0, 0);
			imm_context->VSSetConstantBuffers(1, 1, m_pConstantBufferMaterial.GetAddressOf());
			imm_context->HSSetConstantBuffers(1, 1, m_pConstantBufferMaterial.GetAddressOf());
			imm_context->DSSetConstantBuffers(1, 1, m_pConstantBufferMaterial.GetAddressOf());
			imm_context->GSSetConstantBuffers(1, 1, m_pConstantBufferMaterial.GetAddressOf());
			imm_context->PSSetConstantBuffers(1, 1, m_pConstantBufferMaterial.GetAddressOf());


			subset.diffuse.texture->Set(imm_context);

			imm_context->DrawIndexed(subset.indexCount, subset.indexStart, 0);
		}
	}
	mFrameInterpolation.Tick();

}

//----------------------------------------------------------------------------------------------------------------------------

bool SkinnedMesh::AddMotion(std::string& name, const char* filename)
{
	if (m_meshes.size() == 0) return false;

	std::unique_ptr<FbxLoader> loader = std::make_unique<FbxLoader>();

	return (loader->AddMotion(name, filename, m_meshes));
}

//----------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX SkinnedMesh::Lerp(DirectX::XMFLOAT4X4& A, DirectX::XMFLOAT4X4& B, float rate)
{
	DirectX::XMMATRIX out;
	out = DirectX::XMLoadFloat4x4(&A) * (1.0f - rate) + DirectX::XMLoadFloat4x4(&B) * rate;
	//DirectX::XMFLOAT4X4 outt;
	//DirectX::XMStoreFloat4x4(&outt, out);
	return out;
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinnedMesh::Play(std::string name, bool isLooped)
{
	auto it = m_meshes.begin();
	while (it != m_meshes.end())
	{
		auto i = it->m_motions.find(name);
		if (i == it->m_motions.end())
		{
			std::cout << "the motion not found." << std::endl;
			return;
		}
		i->second.bIsLooped = isLooped;
		it->m_motions[mCurrentMotion].animeTick = 0.0f;
		it++;
	}

	mFrameInterpolation.Init();
	mPreviousMotion = mCurrentMotion;
	mCurrentMotion = name;

	std::cout << "Now Playing : " << mCurrentMotion << std::endl;
	std::cout << "Played : " << mPreviousMotion << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX SkinnedMesh::CalculateMotionMatrix(int frame, int bone_id, MyFbxMesh& mesh)
{
	DirectX::XMFLOAT4X4& current = mesh.m_motions[mCurrentMotion].keys.at(bone_id).at(frame);
	if (mFrameInterpolation.IsStopped()) return XMLoadFloat4x4(&current);

	//std::cout << "Interpolation : " << mFrameInterpolation.GetTime() << std::endl;

	unsigned int anotherFrame = frame;
	FbxInfo::Motion& previousMotion = mesh.m_motions[mPreviousMotion];
	if (frame > previousMotion.frameSize) anotherFrame = frame - (previousMotion.frameSize);

	DirectX::XMFLOAT4X4 previous = mesh.m_motions[mPreviousMotion].keys.at(bone_id).at(anotherFrame);



	return Lerp(previous, current, mFrameInterpolation.GetTimeRate());
}


//----------------------------------------------------------------------------------------------------------------------------

