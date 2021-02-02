#include "NewMeshRenderer.h"

#include <DirectXMath.h>

#include "RenderTarget.h"
#include "DepthStencilView.h"
#include "GeometricPrimitiveSelf.h"
#include "Shader.h"
#include "SkinnedMesh.h"
//#include "Skybox.h"
#include "StaticMesh.h"
//#include "TextureHolder.h"
#include "Renderer.h"
#include "Plane.h"
//#include "ComputedTexture.h"
#include "GraphicsDevice.h"
#include "ShaderInterop_Renderer.h"

#include "./Engine/Actor.h"
//#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/GameSystem.h"

//#include "./Component/MeshComponent.h"


#include "./RenderPass/ForwardPasses.h"

#include "./Utilities/Log.h"


//--------------------------------------------------------------------------------------------------------------------------------
//http://texturify.com/category/environment-panoramas.html
//http://www.hdrlabs.com/sibl/

NewMeshRenderer::NewMeshRenderer()
	:mSkyboxFilename(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg"),
	mpSkybox(std::make_unique<Skybox>())
{
}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshRenderer::CreateConstantBuffers(Graphics::GraphicsDevice* p_device)
{
	//Graphics::GPUBufferDesc desc = {};
	//desc.ByteWidth = sizeof(CBufferForMesh);
	//desc.BindFlags = Graphics::BIND_CONSTANT_BUFFER;
	//desc.Usage = Graphics::USAGE_DEFAULT;
	//desc.CPUAccessFlags = 0;
	//desc.MiscFlags = 0;
	//desc.StructureByteStride = 0;

	//p_device->CreateBuffer(&desc, nullptr, mpCBufferForMesh.get());
}

bool NewMeshRenderer::Initialize(Graphics::GraphicsDevice* p_device)
{
	mpSkybox->Initialize(p_device, mSkyboxFilename.c_str());
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool NewMeshRenderer::InitializeMeshes(Graphics::GraphicsDevice* p_device)
{
	auto D3DDevice = p_device->GetDevicePtr();
	for (auto& component : mMeshDataTable)
	{
		switch (component.second->GetMeshTypeID())
		{
		case MeshTypeID::E_BasicCube:
			mMeshTable.emplace(component.first, std::make_unique<BasicCube>(p_device));
			break;
		case MeshTypeID::E_BasicCylinder:
			mMeshTable.emplace(component.first, std::make_unique<BasicCylinder>(p_device));
			break;
		case MeshTypeID::E_BasicSphere:
			mMeshTable.emplace(component.first, std::make_unique<BasicSphere>(p_device, 20, 20));
			break;
		case MeshTypeID::E_BasicCapsule:
			mMeshTable.emplace(component.first, std::make_unique<BasicSphere>(p_device));
			break;
		case MeshTypeID::E_BasicLine:
			mMeshTable.emplace(component.first, std::make_unique<BasicLine>(p_device));
			break;
		case MeshTypeID::E_Plane:
			mMeshTable.emplace(component.first, std::make_unique<Plane>(p_device, component.second->GetMeshFileName().c_str()));
			break;
		case MeshTypeID::E_StaticMesh:
			mMeshTable.emplace(component.first, std::make_unique<StaticMesh>(p_device, component.second->GetMeshFileName().c_str(), true));
			break;
		case MeshTypeID::E_SkinnedMesh:
		{
			char file_multibyte[256];
			WideCharToMultiByte(CP_ACP, 0, component.second->GetMeshFileName().c_str(), -1, file_multibyte, 256, NULL, NULL);

			if (file_multibyte == nullptr) Log::Error("[MESH RENDERER] Couldn't regist mesh because filename is empty.(ActorID : %d)", component.first);

			std::unique_ptr<SkinnedMesh> mesh = std::make_unique<SkinnedMesh>(p_device, file_multibyte, component.second->GetFbxType());
			const std::unordered_map<std::string, std::wstring> table = component.second->GetMotionTable();
			if (!table.empty())
			{
				for (auto& data : table)
				{
					char motion_multibyte[256];
					WideCharToMultiByte(CP_ACP, 0, data.second.c_str(), -1, motion_multibyte, 256, NULL, NULL);
					std::string name = data.first;
					mesh->AddMotion(name, motion_multibyte);
				}
			}
			mMeshTable.emplace(component.first, std::move(mesh));
		}
		break;
		default:
			Log::Warning("Couldn't create Mesh. it doesn't exist.");
			break;
		}
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshRenderer::RegistMeshDataFromActors(Graphics::GraphicsDevice* device, ActorManager* p_actors)
{
	for (auto i = 0u; i < p_actors->GetActorsSize(); ++i)
	{
		if (mMeshDataTable.find(i) != mMeshDataTable.end()) continue;
		mMeshDataTable.emplace(i, p_actors->GetActor(i)->GetComponent<MeshComponent>());
	}

	InitializeMeshes(device);
}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshRenderer::Render(Graphics::GraphicsDevice* device, float elapsed_time, unsigned int current_pass_id)
{
	D3D::DeviceContextPtr pImmContext = device->GetImmContextPtr();

	device->OMSetDepthStencilState(Graphics::DS_TRUE_LESS_EQUAL);

	RenderSkybox(device, elapsed_time, current_pass_id);

	device->OMSetDepthStencilState(Graphics::DS_TRUE);

	RenderMesh(device, elapsed_time, current_pass_id);
}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshRenderer::RenderMesh(Graphics::GraphicsDevice* p_device, float elapsed_time, unsigned int current_pass_id)
{
	auto pImmContext = p_device->GetImmContextPtr();
	for (auto& mesh : mMeshTable)
	{
		// ���b�V���f�[�^�󂯎���
		MeshComponent* component = mMeshDataTable.find(mesh.first)->second;

		// ���b�V���`���p�����̏���
		DirectX::XMMATRIX world = component->GetWorldMatrix();
		bool bIsSolid = component->GetIsSolid();
		ShaderID id = static_cast<ShaderID>(component->GetShaderID(ChooseShaderUsageForMesh(current_pass_id)));
		if (id == ShaderID::UNREGISTERED_SHADER) continue;

		ENGINE.GetRenderer()->GetRenderPass(current_pass_id)->SetShader(p_device, static_cast<ShaderID>(id));
		auto textureHolder = ENGINE.GetRenderer()->GetTextureHolderPtr();

		// Set Texture Info
		for (auto& texData : component->GetTextureTable())
		{
			if (texData.filename == L"EMPTY") textureHolder->Set(pImmContext, texData.filename.c_str(), texData.slot, false);
			else							  textureHolder->Set(pImmContext, texData.filename.c_str(), texData.slot);
		}

		p_device->SetSamplers(Graphics::SS_LINEAR_WRAP, 0);
		p_device->SetSamplers(Graphics::SS_ANISO_CLAMP, 1);
		p_device->SetSamplers(Graphics::SS_LINEAR_CLAMP, 1);

		// Set Motion
		// ���[�V�������ύX�����Ă�����
		if (component->GetWasChangedMotion())
		{
			// ���[�V�����Đ�
			Mesh* meshptr = mesh.second.release();
			static_cast<SkinnedMesh*>(meshptr)->Play(component->GetCurrentName(), component->GetAnimeBlendTime());
			mesh.second.reset(meshptr);
			component->DeactivateChangedMotion();
		}
		// ���݂�ShadowPass�Ȃ��Ashadow�̕`�施�߂𑗂�
		bool bForShadow = current_pass_id == RenderPassID::EShadowPass ? true : false;

		mesh.second->Render(p_device, elapsed_time, world, ENGINE.GetCameraPtr().get(), nullptr, component->GetMaterialData(), bForShadow, bIsSolid);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshRenderer::RenderSkybox(Graphics::GraphicsDevice* p_device, float elapsed_time, unsigned int current_pass_id)
{
	auto pImmContext = p_device->GetImmContextPtr();
	if (!mpSkybox->HasComputed()) mpSkybox->ConvertEquirectToCubeMap(p_device);

	UINT shaderID = static_cast<ShaderID>(ChooseShaderIdForSkybox(current_pass_id));

	ENGINE.GetRenderer()->GetRenderPass(current_pass_id)->SetShader(p_device, static_cast<ShaderID>(shaderID));

	Vector3 pos = ENGINE.GetCameraPtr()->GetCameraPosition();

	DirectX::XMMATRIX w = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	Material data = {};

	mpSkybox->Render(p_device, elapsed_time, w, ENGINE.GetCameraPtr().get(), nullptr, data);
}

//--------------------------------------------------------------------------------------------------------------------------------


UINT NewMeshRenderer::ChooseShaderUsageForMesh(UINT current_pass)
{
	switch (current_pass)
	{
	case RenderPassID::EShadowPass:
		return static_cast<UINT>(ShaderUsage::EShader);
	case RenderPassID::ECubeMapPass:
		return static_cast<UINT>(ShaderUsage::ECubeMap);
	default:
		return static_cast<UINT>(ShaderUsage::EMain);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

UINT NewMeshRenderer::ChooseShaderIdForSkybox(UINT current_pass)
{
	switch (current_pass)
	{
	case RenderPassID::EForwardPass:
		return ShaderID::ESkyboxRevised;
	case RenderPassID::EDeferredPass:
		return ShaderID::EDefferedSkyboxRevised;
	case RenderPassID::ECubeMapPass:
		return ShaderID::EMakeCubeMap;
	default:
		Log::Warning("Inserted invalid number.");
		return ShaderID::ESkybox;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshRenderer::RenderUI()
{
	mpSkybox->RenderUI();
}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshRenderer::ClearAll()
{
	mMeshDataTable.clear();
	mMeshTable.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------

ComputedTexture* NewMeshRenderer::GetCurrentSkyTex()
{
	return mpSkybox->GetTexturePtr();
}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshRenderer::SetSkybox(int id)
{
	mpSkybox->SetCurrentSkybox(id);
}

//--------------------------------------------------------------------------------------------------------------------------------
