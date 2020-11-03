#include "MeshComponent.h"

#include "./Component/ColliderComponent.h"

#include "./Engine/MeshRenderer.h"
#include "./Engine/Actor.h"
#include "./Engine/CameraController.h"
#include "./Engine/LightController.h"

#include "./Renderer/Shader.h"
#include "./Renderer/Mesh.h"
#include "./Renderer/SkinnedMesh.h"
#include "./Renderer/Texture.h"

#include "./Utilities/MyArrayFromVector.h"
#include "./Utilities/ImGuiSelf.h"

//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;


//----------------------------------------------------------------------------------------------------------------------------
#pragma region OLDIMPL

//MeshComponent::MeshComponent(const std::shared_ptr<Actor>& owner)
//	:
//	Component(owner), 
//	m_pMesh(nullptr),
//	m_pShader(nullptr),
//	m_pShaderForShadow(nullptr),
//	mMeshId(MeshID::EBasicSphere),
//	mMatColor(Vector4(1, 1, 1, 1)),
//	m_bIsSolid(true)
//{
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//MeshComponent::~MeshComponent()
//{
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//std::shared_ptr<MeshComponent> MeshComponent::Initialize(const std::shared_ptr<Actor>& owner)
//{
//	return std::shared_ptr<MeshComponent>(new MeshComponent(owner));
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//bool MeshComponent::Create()
//{
//	return true;
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//void MeshComponent::Destroy()
//{
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//void MeshComponent::Update(float elapsed_time)
//{
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//void MeshComponent::RenderMesh(
//	D3D::DeviceContextPtr& imm_context,
//	float elapsed_time,
//	const CameraPtr& camera)
//{
//	if (mIsValid == false) return;
//
//	SetAdditionalResource(imm_context);
//
//	XMMATRIX world = m_pOwner.lock()->GetWorldMatrix();
//	if (m_pColData != nullptr) world = m_pColData->GetWorldMatrix();
//	m_pMesh->Render(imm_context, elapsed_time, world, camera.get(), m_pShader.get(), mMatColor, false, m_bIsSolid);
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//void MeshComponent::RenderShadow(
//	D3D::DeviceContextPtr& imm_context,
//	float elapsed_time, 
//	const CameraPtr& p_camera)
//{
//	if (m_pShaderForShadow == nullptr) return;
//
//	XMMATRIX world = m_pOwner.lock()->GetWorldMatrix();
//	if (m_pColData != nullptr) world = m_pColData->GetWorldMatrix();
//	m_pMesh->Render(imm_context, elapsed_time, world, p_camera.get(), m_pShaderForShadow.get(), mMatColor, true, m_bIsSolid);
//
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//bool MeshComponent::Load(int mesh_id, const std::shared_ptr<MeshRenderer>& renderer)
//{
//	std::shared_ptr<Component> component = std::enable_shared_from_this<Component>::shared_from_this();
//	renderer->AddMesh(mesh_id, std::static_pointer_cast<MeshComponent>(component));
//	mMeshId = static_cast<MeshID>(mesh_id);
//	return true;
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//bool MeshComponent::Load(const char* filename, int mesh_id, const std::shared_ptr<MeshRenderer>& renderer)
//{
//	std::shared_ptr<Component> component = std::enable_shared_from_this<Component>::shared_from_this();
//	renderer->AddMesh(filename, mesh_id, std::static_pointer_cast<MeshComponent>(component));
//	mMeshId = static_cast<MeshID>(mesh_id);
//
//	return true;
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//bool MeshComponent::Load(const wchar_t* filename, int mesh_id, const std::shared_ptr<MeshRenderer>& renderer)
//{
//	std::shared_ptr<Component> component = std::enable_shared_from_this<Component>::shared_from_this();
//	renderer->AddMesh(filename, mesh_id, std::static_pointer_cast<MeshComponent>(component));
//	mMeshId = static_cast<MeshID>(mesh_id);
//
//	return true;
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//bool MeshComponent::AddShaderResource(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, const wchar_t* filename, unsigned int slot)
//{
//	std::shared_ptr<Texture> pShaderResource = std::make_shared<Texture>();
//	pShaderResource->Load(pDevice, filename);
//	m_pTextureTable.emplace(std::pair(slot, pShaderResource));
//
//	return true;
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//bool MeshComponent::AddMotion(std::string name, const char* filename)
//{
//	if (!(mMeshId == MeshID::ESkinnedMesh)) return false;
//
//	return std::static_pointer_cast<SkinnedMesh>(m_pMesh)->AddMotion(name, filename);
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//void MeshComponent::Play(std::string name)
//{
//	std::static_pointer_cast<SkinnedMesh>(m_pMesh)->Play(name);
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//void MeshComponent::SetAdditionalResource(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context)
//{
//	if (m_pTextureTable.empty()) return;
//
//	for (auto& texture : m_pTextureTable)
//	{
//		texture.second->Set(imm_context, texture.first);
//	}
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//void MeshComponent::SetTopology(D3D11_PRIMITIVE_TOPOLOGY type)
//{
//	m_pMesh->SetTopology(type);
//}
//
////----------------------------------------------------------------------------------------------------------------------------
#pragma endregion

//--------------------------------------------------------------------------------------------------------------------------------

const std::string NewMeshComponent::MESH_TYPE_NAMES[MeshTypeID::ENUM_MESH_TYPE_MAX] = {
	"None",
	"BasicCube",
	"BasicCylinder",
	"BasicSphere",
	"BasicCapsule",
	"BasicLine",
	"Plane",
	"StaticMesh",
	"SkinnedMesh",
};

//--------------------------------------------------------------------------------------------------------------------------------

NewMeshComponent::NewMeshComponent(const std::shared_ptr<Actor>& p_owner)
	:Component(p_owner),
	mbIsSolid(true),
	mCurrentMotionKey("default"),
	mbChangedMotion(false)
{
	mMeshID = MeshTypeID::E_Default;
	mMaterialData.mat_color = { 1, 1, 1, 1 };
	mMaterialData.brdfFactor = 1.0f;
	mMaterialData.metalness = 1.0f;
	mMaterialData.roughness = 0.0f;
	mMaterialData.specularColor = { 1.0f, 1.0f, 1.0f };
}

//--------------------------------------------------------------------------------------------------------------------------------

bool NewMeshComponent::Create()
{
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshComponent::Destroy()
{
}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshComponent::Update(float elapsed_time)
{

}

//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshComponent::Play(const char* name)
{
	mCurrentMotionKey = name;
	mbChangedMotion = true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool NewMeshComponent::RegistMesh(MeshTypeID mesh_type_id, ShaderType shader_id, const wchar_t* mesh_filename, unsigned int coord_system)
{
	mMeshID = mesh_type_id < MeshTypeID::ENUM_MESH_TYPE_MAX ? mesh_type_id : MeshTypeID::E_Default;
	mFbxType = coord_system;
	mShaderIDTable.emplace_back(shader_id);
	if (mesh_filename != nullptr) mMeshFileName = mesh_filename;
	else
	{
		TextureData data = {};
		data.slot = 0;
		data.filename = L"DUMMY";

		mTextureTable.emplace_back(data);
	}

	Log::Info("[MESH COMP] Regist Mesh ( ActorNo.%d ). ( MeshType : %s, ShaderType : %d, Filename : %s )",OwnerPtr->GetID(),  MESH_TYPE_NAMES[static_cast<u_int>(mMeshID)].c_str(), shader_id, mMeshFileName.c_str());
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------


bool NewMeshComponent::RegistTexture(const wchar_t* tex_filename, unsigned int slot)
{
	if (tex_filename == nullptr)
	{
		Log::Error("[MESH COMP] Failed to regist. tex_filename is nullptr.");
		assert(tex_filename != nullptr);
	}

	
	TextureData data = {};
	data.slot = slot;
	data.filename = tex_filename;

	mTextureTable.emplace_back(data);
	Log::Info("[MESH COMP] Regist Texture (ActorNo.%d). ( Slot : %d)", OwnerPtr->GetID(), MESH_TYPE_NAMES[mMeshID], slot/*, mMeshFileName*/);
	return true;
}

bool NewMeshComponent::RegistMotion(const char* name, const wchar_t* motion_filename)
{
	mMotionFileTable[name] = motion_filename;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool NewMeshComponent::RegistAdditionalShader(ShaderType shader_id, unsigned int usage)
{
	mShaderIDTable.emplace_back(shader_id);
	Log::Info("[MESH COMP] Regist Additional Shader (ActorNo.%d). ( Usage : %d, ShaderType : %d", OwnerPtr->GetID(), usage, shader_id);
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

const DirectX::XMMATRIX& NewMeshComponent::GetWorldMatrix()
{
	return OwnerPtr->GetWorldMatrix();
}

const DirectX::XMFLOAT4& NewMeshComponent::GetMaterialColor()
{
	return mMaterialData.mat_color;
}

//--------------------------------------------------------------------------------------------------------------------------------

ShaderType               NewMeshComponent::GetShaderID(u_int usage)
{
	if (usage == 1 && mShaderIDTable.size() == 1)
	{
		Log::Error("[MESH COMPONENT] Please register shader for SHADOW with mesh.");
	}
	
	return mShaderIDTable[usage];
}
//--------------------------------------------------------------------------------------------------------------------------------

void NewMeshComponent::SetMaterialColor(const Vector4& color)
{
	mMaterialData.mat_color = color;
}

void NewMeshComponent::SetBRDFFactors(float metalness, float roughness, const Vector3& specColor)
{
	mMaterialData.metalness = metalness;
	mMaterialData.roughness = roughness;
	mMaterialData.specularColor = specColor;
}


void NewMeshComponent::RenderUI()
{
	ImGui::SliderFloat("Metalness", &mMaterialData.metalness, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &mMaterialData.roughness, 0.0f, 1.0f);
	MyArrayFromVector specColor = MyArrayFromVector(mMaterialData.specularColor);
	ImGui::SliderFloat3("Specular Color", specColor.SetArray(), 0.0f, 1.0f);
	ImGui::SliderFloat("BRDF Specular Factor", &mMaterialData.brdfFactor, 0.0f, 1.0f);
	ImGui::Separator();
}

std::shared_ptr<NewMeshComponent> NewMeshComponent::Initialize(const std::shared_ptr<Actor>& p_owner)
{
	return std::shared_ptr<NewMeshComponent>(new NewMeshComponent(p_owner));
}

//--------------------------------------------------------------------------------------------------------------------------------
