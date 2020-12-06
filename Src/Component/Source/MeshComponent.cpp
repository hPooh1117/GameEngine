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

#include "./RenderPass/ShaderIDTable.h"

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
//std::shared_ptr<MeshComponent> MeshComponent::Create(const std::shared_ptr<Actor>& owner)
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
//	XMMATRIX world = mpOwner.lock()->GetWorldMatrix();
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
//	XMMATRIX world = mpOwner.lock()->GetWorldMatrix();
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


const std::string MeshComponent::MESH_TYPE_NAMES[MeshTypeID::ENUM_MESH_TYPE_MAX] = {
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

MeshComponent::MeshComponent(Actor* p_owner)
	:Component(p_owner),
	mbIsSolid(true),
	mCurrentMotionKey("default"),
	mbChangedMotion(false),
	mAnimeBlendTime(60),
	mbIsPBR(false)
{
	mMeshID = MeshTypeID::E_Default;
	mMaterialData.mat_color = { 1, 1, 1, 1 };
	mMaterialData.textureConfig = 0;
	mMaterialData.metalness = 1.0f;
	mMaterialData.roughness = 0.0f;
	mMaterialData.diffuse = 1.0f;
	mMaterialData.specular = 1.0f;
	mMaterialData.specularColor = { 1.0f, 1.0f, 1.0f };
}

//--------------------------------------------------------------------------------------------------------------------------------

bool MeshComponent::Create()
{
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void MeshComponent::Destroy()
{
}

//--------------------------------------------------------------------------------------------------------------------------------

void MeshComponent::Update(float elapsed_time)
{

}

//--------------------------------------------------------------------------------------------------------------------------------

void MeshComponent::Play(const char* name)
{
	mCurrentMotionKey = name;
	mbChangedMotion = true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool MeshComponent::RegisterMesh(UINT mesh_type_id, UINT shader_id, const wchar_t* mesh_filename, UINT coord_system)
{
	mMeshID = mesh_type_id < MeshTypeID::ENUM_MESH_TYPE_MAX ? mesh_type_id : MeshTypeID::E_Default;
	mFbxType = coord_system;
	mShaderIDTable.emplace(ShaderUsage::EMain, shader_id);
	if (mesh_filename != nullptr)
	{
		mMeshFileName = mesh_filename;
		mTextureConfig += static_cast<int>(TextureConfig::EColorMap);
	}
	else
	{
		TextureData data = {};
		data.slot = 0;
		data.filename = L"DUMMY";

		mTextureTable.emplace_back(data);
	}

	Log::Info("[MESH COMP] Regist Mesh ( ActorNo.%d ). ( MeshType : %s, ShaderType : %d )", mpOwner->GetID(),  MESH_TYPE_NAMES[static_cast<u_int>(mMeshID)].c_str(), shader_id);
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------


bool MeshComponent::RegisterTexture(const wchar_t* tex_filename, TextureConfig textureConfig)
{
	if (tex_filename == nullptr)
	{
		Log::Error("[MESH COMP] Failed to regist. tex_filename is nullptr.");
		assert(tex_filename != nullptr);
	}

	
	
	TextureData data = {};
	data.slot = static_cast<unsigned int>(log2f(static_cast<float>(textureConfig)));
	data.filename = tex_filename;

	mTextureConfig += static_cast<int>(textureConfig);
	Log::Info("[MESH COMP] Current TextureConfig is %d", mTextureConfig);
	mMaterialData.textureConfig = mTextureConfig;

	if (textureConfig == TextureConfig::EMetallicMap || textureConfig == TextureConfig::ERoughnessMap) mbIsPBR = true;

	mTextureTable.emplace_back(data);
	Log::Info("[MESH COMP] Regist Texture (ActorNo.%d). ( Slot : %d)", mpOwner->GetID(), data.slot/*, mMeshFileName*/);
	return true;
}

bool MeshComponent::RegisterMotion(const char* name, const wchar_t* motion_filename)
{
	mMotionFileTable[name] = motion_filename;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

bool MeshComponent::RegisterAdditionalShader(ShaderID shader_id, unsigned int usage)
{
	mShaderIDTable.emplace(usage, shader_id);
	Log::Info("[MESH COMP] Regist Additional Shader (ActorNo.%d). ( Usage : %d, ShaderType : %d", mpOwner->GetID(), usage, shader_id);
	if (usage == ShaderUsage::EShader) mTextureConfig += static_cast<int>(TextureConfig::EShadowMap);
	mMaterialData.textureConfig = mTextureConfig;
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------

const DirectX::XMMATRIX& MeshComponent::GetWorldMatrix()
{
	return mpOwner->GetWorldMatrix();
}

const DirectX::XMFLOAT4& MeshComponent::GetMaterialColor()
{
	return mMaterialData.mat_color;
}

//--------------------------------------------------------------------------------------------------------------------------------

UINT    MeshComponent::GetShaderID(u_int usage)
{
	if (usage == 1 && mShaderIDTable.size() == 1)
	{
		Log::Error("[MESH COMPONENT] Please register shader for SHADOW with mesh.");
	}
	
	if (mShaderIDTable.find(usage) != mShaderIDTable.end()) return mShaderIDTable[usage];
	else                                                    return ShaderID::UNREGISTERED_SHADER;
}
//--------------------------------------------------------------------------------------------------------------------------------

void MeshComponent::SetMaterialColor(const Vector4& color)
{
	mMaterialData.mat_color = color;
}

void MeshComponent::SetMaterialColor(const Vector3& color)
{
	mMaterialData.mat_color.x = color.x;
	mMaterialData.mat_color.y = color.y;
	mMaterialData.mat_color.z = color.z;
}

void MeshComponent::SetBRDFFactors(float metalness, float roughness, const Vector3& specColor)
{
	mMaterialData.metalness = metalness;
	mMaterialData.roughness = roughness;
	mMaterialData.specularColor = specColor;
	mbIsPBR = true;
}


void MeshComponent::RenderUI()
{
	if (mMeshID == MeshTypeID::E_SkinnedMesh) ImGui::SliderInt("AnimeBlendTime", &mAnimeBlendTime, 30, 240);

	if (!mbIsPBR) return;

	static int metallicFlag = 0;
	ImGui::RadioButton("Metal", &metallicFlag, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Dielectric", &metallicFlag, 0);
	mMaterialData.metalness = static_cast<float>(metallicFlag);

	ImGui::SliderFloat("Roughness", &mMaterialData.roughness, 0.0f, 1.0f);
	
	static bool diffuseFlag = true;
	ImGui::Checkbox("Diffuse", &diffuseFlag);
	mMaterialData.diffuse = static_cast<float>(diffuseFlag);
	
	static bool specularFlag = true;
	ImGui::Checkbox("Specular", &specularFlag);
	mMaterialData.specular = static_cast<float>(specularFlag);

	MyArrayFromVector color = MyArrayFromVector(mMaterialData.mat_color);
	ImGui::ColorPicker4("Diffuse Color", color.SetArray());
}

//--------------------------------------------------------------------------------------------------------------------------------
