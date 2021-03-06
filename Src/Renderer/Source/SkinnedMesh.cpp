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

SkinnedMesh::SkinnedMesh(Graphics::GraphicsDevice* p_device, const char* filename, unsigned int coord_system)
	:Mesh(),
	mFrameInterpolation(MAX_MOTION_INTERPOLATION),
	mCoordSystem(coord_system)
{
	LoadFbxFile(p_device, filename);

	mFrameInterpolation.SetTime(MAX_MOTION_INTERPOLATION);


	HRESULT hr = S_OK;
	auto& pDevice = p_device->GetDevicePtr();

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
			sizeof(CBufferForBone),
			D3D11_BIND_CONSTANT_BUFFER,
			D3D11_USAGE_DEFAULT,
			0, 0, 0
		),
		nullptr,
		m_constant_buffer_bone.GetAddressOf()
	);

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

}

//----------------------------------------------------------------------------------------------------------------------------

void SkinnedMesh::LoadFbxFile(Graphics::GraphicsDevice* p_device, const char* filename)
{
	auto& pDevice = p_device->GetDevicePtr();

	std::unique_ptr<FbxLoader> fbxLoader = std::make_unique<FbxLoader>();

	std::chrono::system_clock::time_point start, end;
	start = std::chrono::system_clock::now();

	if (fbxLoader->Load(pDevice, filename, mMeshes))
	{
		CreateBuffers(p_device);
	}
	end = std::chrono::system_clock::now();

#ifdef _DEBUG
	double elapsed = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

	Log::Info("[SKINNED MESH] Loaded FBX Mesh. Filename: %s (%.2f ms)", filename, elapsed);
#endif
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinnedMesh::CreateBuffers(Graphics::GraphicsDevice* p_device)
{
	for (auto& mesh : mMeshes)
	{
		// VERTEX BUFFER
		Graphics::GPUBufferDesc desc = {};
		desc.ByteWidth = sizeof(FbxInfo::Vertex) * mesh.mVertices.size();
		desc.Usage = Graphics::USAGE_IMMUTABLE;
		desc.BindFlags = Graphics::BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		mesh.mpVertexBuffer->desc = desc;

		Graphics::SubresourceData data = {};
		data.pSysMem = mesh.mVertices.data();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		p_device->CreateBuffer(&desc, &data, mesh.mpVertexBuffer.get());

		//INDEX BUFFER
		desc.ByteWidth = sizeof(uint32_t) * mesh.mIndices.size();
		desc.BindFlags = Graphics::BIND_INDEX_BUFFER;
		mesh.mpIndexBuffer->desc = desc;
		data.pSysMem = mesh.mIndices.data();
		p_device->CreateBuffer(&desc, &data, mesh.mpIndexBuffer.get());
	}
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinnedMesh::Render(
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
	if (shader != nullptr)
	{
		shader->ActivateShaders(ImmContext);
	}

	DirectX::XMFLOAT4X4 W, WVP;
	CBufferForMesh meshData = {};
	DirectX::XMStoreFloat4x4(&W, world);
	DirectX::XMStoreFloat4x4(&WVP, world * (isShadow ? camera->GetOrthoView() * camera->GetOrthoProj(ImmContext) : camera->GetViewMatrix() * camera->GetProjMatrix(ImmContext)));
	DirectX::XMStoreFloat4x4(&meshData.invViewProj, camera->GetInvProjMatrix(ImmContext));
	DirectX::XMStoreFloat4x4(&meshData.invView, camera->GetInvViewMatrix());
	DirectX::XMStoreFloat4x4(&meshData.invProj, camera->GetInvProjMatrix(ImmContext));


	for (auto& mesh : mMeshes)
	{
		CBufferForBone boneData = {};

		UINT stride = sizeof(FbxInfo::Vertex);
		UINT offset = 0;




		if (mesh.mMotions[mCurrentMotion].frameSize > 0)
		{
			FbxInfo::Motion& motion = mesh.mMotions[mCurrentMotion];
			auto frame = static_cast<unsigned int>(motion.animeTick / FbxInfo::SAMPLE_TIME);
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

		Graphics::GPUBuffer* vb = mesh.mpVertexBuffer.get();
		p_device->BindVertexBuffer(&vb, 0, 1, &stride, &offset);
		p_device->BindIndexBuffer(mesh.mpIndexBuffer.get(), Graphics::INDEXFORMAT_32BIT, 0);
		p_device->BindPrimitiveTopology(Graphics::TRIANGLELIST);
		if (isSolid)
		{
			p_device->RSSetState(mCoordSystem > 0 ? Graphics::RS_MAYA_BACK : Graphics::RS_BACK);
		}
		else
		{
			p_device->RSSetState(mCoordSystem > 0 ? Graphics::RS_MAYA_WIRE_BACK : Graphics::RS_WIRE_BACK);
		}

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

		ImmContext->UpdateSubresource(mpConstantBufferMesh.Get(), 0, nullptr, &meshData, 0, 0);
		ImmContext->VSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
		ImmContext->HSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
		ImmContext->DSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
		ImmContext->GSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());
		ImmContext->PSSetConstantBuffers(0, 1, mpConstantBufferMesh.GetAddressOf());

		ImmContext->UpdateSubresource(m_constant_buffer_bone.Get(), 0, nullptr, &boneData, 0, 0);
		ImmContext->VSSetConstantBuffers(4, 1, m_constant_buffer_bone.GetAddressOf());
		ImmContext->PSSetConstantBuffers(4, 1, m_constant_buffer_bone.GetAddressOf());

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
			matData.textureConfig = mat_data.textureConfig;
			matData.diffuse = mat_data.diffuse;
			matData.specular = mat_data.specular;

			ImmContext->UpdateSubresource(mpConstantBufferMaterial.Get(), 0, nullptr, &matData, 0, 0);
			ImmContext->VSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
			ImmContext->HSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
			ImmContext->DSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
			ImmContext->GSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());
			ImmContext->PSSetConstantBuffers(1, 1, mpConstantBufferMaterial.GetAddressOf());

			for (auto& tex : subset.diffuse.textures)
			{
				if (tex) tex->Set(ImmContext);
			}

			p_device->DrawIndexed(subset.indexCount, subset.indexStart, 0);
		}
	}
	mFrameInterpolation.Tick();

}

//----------------------------------------------------------------------------------------------------------------------------

bool SkinnedMesh::AddMotion(std::string& name, const char* filename)
{
	if (mMeshes.size() == 0) return false;

	std::unique_ptr<FbxLoader> loader = std::make_unique<FbxLoader>();

	return (loader->AddMotion(name, filename, mMeshes));
}

//----------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX SkinnedMesh::Lerp(DirectX::XMFLOAT4X4& A, DirectX::XMFLOAT4X4& B, float rate)
{
	DirectX::XMMATRIX out;
	out = DirectX::XMLoadFloat4x4(&A) * (1.0f - rate) + DirectX::XMLoadFloat4x4(&B) * rate;
	return out;
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinnedMesh::Play(std::string name, int blend_time, bool isLooped)
{
	auto it = mMeshes.begin();
	while (it != mMeshes.end())
	{
		auto i = it->mMotions.find(name);
		if (i == it->mMotions.end())
		{
			std::cout << "the motion not found." << std::endl;
			return;
		}
		i->second.bIsLooped = isLooped;
		it->mMotions[mCurrentMotion].animeTick = 0.0f;
		it++;
	}

	mFrameInterpolation.SetMaxTime(blend_time);
	mFrameInterpolation.Init();
	mPreviousMotion = mCurrentMotion;
	mCurrentMotion = name;

	std::cout << "Now Playing : " << mCurrentMotion << std::endl;
	std::cout << "Played : " << mPreviousMotion << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX SkinnedMesh::CalculateMotionMatrix(int frame, int bone_id, MyFbxMesh& mesh)
{
	DirectX::XMFLOAT4X4& current = mesh.mMotions[mCurrentMotion].keys.at(bone_id).at(frame);
	if (mFrameInterpolation.IsStopped()) return XMLoadFloat4x4(&current);

	//std::cout << "Interpolation : " << mFrameInterpolation.GetTime() << std::endl;

	unsigned int anotherFrame = frame;
	FbxInfo::Motion& previousMotion = mesh.mMotions[mPreviousMotion];
	if (frame > previousMotion.frameSize) anotherFrame = frame - (previousMotion.frameSize);

	DirectX::XMFLOAT4X4 previous = mesh.mMotions[mPreviousMotion].keys.at(bone_id).at(anotherFrame);



	return Lerp(previous, current, mFrameInterpolation.GetTimeRate());
}


//----------------------------------------------------------------------------------------------------------------------------
