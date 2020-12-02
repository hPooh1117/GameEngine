#include "NewMeshRenderer.h"

#include <DirectXMath.h>

#include "RenderTarget.h"
#include "DepthStencilView.h"
#include "GeometricPrimitiveSelf.h"
#include "GraphicsEngine.h"
#include "Shader.h"
#include "SkinnedMesh.h"
#include "Skybox.h"
#include "StaticMesh.h"
#include "TextureHolder.h"
#include "Plane.h"

#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"
#include "./Engine/CameraController.h"
#include "./Engine/GameSystem.h"

#include "./Component/MeshComponent.h"


#include "./RenderPass/ForwardPasses.h"

#include "./Utilities/Log.h"


//--------------------------------------------------------------------------------------------------------------------------------
//http://texturify.com/category/environment-panoramas.html
//http://www.hdrlabs.com/sibl/

NewMeshRenderer::NewMeshRenderer()
	:mSkyboxFilename(L"./Data/Images/Environment/Footprint_Court/Footprint_Court_8k_TMap.jpg")
{
}

//--------------------------------------------------------------------------------------------------------------------------------

bool NewMeshRenderer::Initialize(D3D::DevicePtr& p_device)
{
	mpSkybox = std::make_unique<Skybox>(p_device, mSkyboxFilename.c_str(), 10, 10);

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool NewMeshRenderer::InitializeMeshes(D3D::DevicePtr& p_device)
{

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

void NewMeshRenderer::RegistMeshDataFromActors(D3D::DevicePtr& p_device, const std::unique_ptr<ActorManager>& p_actors)
{


	for (auto i = 0u; i < p_actors->GetActorsSize(); ++i)
	{
		if (mMeshDataTable.find(i) != mMeshDataTable.end()) continue;
		mMeshDataTable.emplace(i, p_actors->GetActor(i)->GetComponent<NewMeshComponent>());
	}

	InitializeMeshes(p_device);


}

void NewMeshRenderer::RenderSkybox(D3D::DeviceContextPtr& p_imm_context, float elapsed_time, unsigned int current_pass_id)
{
	ShaderID type = static_cast<ShaderID>(ChooseShaderIdForSkybox(current_pass_id));
	ENGINE.GetRenderPass(current_pass_id)->SetShader(p_imm_context, type);
	Vector3 pos = ENGINE.GetCameraPtr()->GetCameraPosition();
	DirectX::XMMATRIX w = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	MaterialData data = {};
	mpSkybox->Render(p_imm_context, elapsed_time, w, ENGINE.GetCameraPtr().get(), nullptr, data);

}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshRenderer::RenderMesh(D3D::DeviceContextPtr& p_imm_context, float elapsed_time, unsigned int current_pass_id)
{
	for (auto& mesh : mMeshTable)
	{
		// メッシュデータ受け取り
		std::shared_ptr<NewMeshComponent> component = mMeshDataTable.find(mesh.first)->second;

		// メッシュ描画用情報の準備
		DirectX::XMMATRIX world = component->GetWorldMatrix();
		bool bIsSolid = component->GetIsSolid();
		ShaderID id = static_cast<ShaderID>(component->GetShaderID(ChooseShaderUsageForMesh(current_pass_id)));
		if (id == ShaderID::UNREGISTERED_SHADER) continue;

		ENGINE.GetRenderPass(current_pass_id)->SetShader(p_imm_context, static_cast<ShaderID>(id));

		// テクスチャ準備
		for (auto& texData : component->GetTextureTable())
		{
			if (texData.filename == L"EMPTY") ENGINE.GetTextureHolderPtr()->Set(p_imm_context, texData.filename, texData.slot, false);
			else							  ENGINE.GetTextureHolderPtr()->Set(p_imm_context, texData.filename, texData.slot);
			ENGINE.GetTextureHolderPtr()->SetSampler(p_imm_context, texData.slot, SamplerID::EWrap);
			ENGINE.GetTextureHolderPtr()->SetSampler(p_imm_context, 1, SamplerID::EBorder);
			ENGINE.GetTextureHolderPtr()->SetSampler(p_imm_context, 2, SamplerID::EClamp);
		}
		// モーションが変更されていたら
		if (component->GetWasChangedMotion())
		{
			// モーション再生
			Mesh* meshptr = mesh.second.release();
			static_cast<SkinnedMesh*>(meshptr)->Play(component->GetCurrentName(), component->GetAnimeBlendTime());
			mesh.second.reset(meshptr);
			component->DeactivateChangedMotion();
		}
		// 現在がShadowPassなら、shadowの描画命令を送る
		bool bForShadow = current_pass_id == RenderPassID::EShadowPass ? true : false;

		mesh.second->Render(p_imm_context, elapsed_time, world, ENGINE.GetCameraPtr().get(), nullptr, component->GetMaterialData(), bForShadow, bIsSolid);
	}
}

UINT NewMeshRenderer::ChooseShaderUsageForMesh(UINT current_pass)
{
	switch (current_pass)
	{
	case RenderPassID::EShadowPass:
		return ShaderUsage::EShader;
	case RenderPassID::ECubeMapPass:
		return ShaderUsage::ECubeMap;
	default:
		return ShaderUsage::EMain;
	}
}

UINT NewMeshRenderer::ChooseShaderIdForSkybox(UINT current_pass)
{
	switch (current_pass)
	{
	case RenderPassID::EForwardPass:
		return ShaderID::ESkybox;
	case RenderPassID::EDeferredPass:
		return ShaderID::EDefferedSkybox;
	case RenderPassID::ECubeMapPass:
		return ShaderID::EMakeCubeMap;
	default:
		Log::Warning("Inserted invalid number.");
		return ShaderID::ESkybox;
	}
}

void NewMeshRenderer::RenderUI()
{
	mpSkybox->RenderUI();
}

void NewMeshRenderer::ClearAll()
{
	mMeshDataTable.clear();
	mMeshTable.clear();
}

void NewMeshRenderer::SetSkybox(int id)
{
	mpSkybox->SetCurrentSkybox(id);
}

//--------------------------------------------------------------------------------------------------------------------------------
