#include "MeshComponent.h"

#include "./Engine/MeshRenderer.h"
#include "./Engine/Actor.h"
#include "./Engine/CameraController.h"
#include "./Engine/LightController.h"

#include "./Renderer/Shader.h"
#include "./Renderer/Mesh.h"
#include "./Renderer/SkinnedMesh.h"
#include "./Renderer/Texture.h"

//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;


//----------------------------------------------------------------------------------------------------------------------------

MeshComponent::MeshComponent(const std::shared_ptr<Actor>& owner) :Component(owner), mMeshId(0)
{
}

//----------------------------------------------------------------------------------------------------------------------------

MeshComponent::~MeshComponent()
{
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<MeshComponent> MeshComponent::Initialize(const std::shared_ptr<Actor>& owner)
{
	return std::shared_ptr<MeshComponent>(new MeshComponent(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool MeshComponent::Create()
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void MeshComponent::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void MeshComponent::Update(float elapsed_time)
{
}

//----------------------------------------------------------------------------------------------------------------------------

void MeshComponent::RenderMesh(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
	float elapsed_time,
	const std::shared_ptr<CameraController>& camera)
{
	if (mIsValid == false) return;

	SetAdditionalResource(imm_context);

	XMMATRIX world = m_pOwner.lock()->GetWorldMatrix();
	if (m_pColData != nullptr) world = m_pColData->GetWorldMatrix();
	m_pMesh->Render(imm_context, elapsed_time, world, camera, m_pShader, mMatColor, false, mIsSolid);
}

//----------------------------------------------------------------------------------------------------------------------------

void MeshComponent::RenderShadow(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context, 
	float elapsed_time, 
	const std::shared_ptr<CameraController>& p_camera)
{
	if (m_pShaderForShadow == nullptr) return;

	XMMATRIX world = m_pOwner.lock()->GetWorldMatrix();
	if (m_pColData != nullptr) world = m_pColData->GetWorldMatrix();
	m_pMesh->Render(imm_context, elapsed_time, world, p_camera, m_pShaderForShadow, mMatColor, true, mIsSolid);

}

//----------------------------------------------------------------------------------------------------------------------------

bool MeshComponent::Load(int mesh_id, const std::shared_ptr<MeshRenderer>& renderer)
{
	std::shared_ptr<Component> component = std::enable_shared_from_this<Component>::shared_from_this();
	renderer->AddMesh(mesh_id, std::static_pointer_cast<MeshComponent>(component));
	mMeshId = mesh_id;
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

bool MeshComponent::Load(const char* filename, int mesh_id, const std::shared_ptr<MeshRenderer>& renderer)
{
	std::shared_ptr<Component> component = std::enable_shared_from_this<Component>::shared_from_this();
	renderer->AddMesh(filename, mesh_id, std::static_pointer_cast<MeshComponent>(component));
	mMeshId = mesh_id;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

bool MeshComponent::Load(const wchar_t* filename, int mesh_id, const std::shared_ptr<MeshRenderer>& renderer)
{
	std::shared_ptr<Component> component = std::enable_shared_from_this<Component>::shared_from_this();
	renderer->AddMesh(filename, mesh_id, std::static_pointer_cast<MeshComponent>(component));
	mMeshId = mesh_id;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

bool MeshComponent::AddShaderResource(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, const wchar_t* filename)
{
	std::shared_ptr<Texture> pShaderResource = std::make_shared<Texture>();
	pShaderResource->Load(pDevice, filename);
	m_pShaderResources.emplace_back(pShaderResource);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

bool MeshComponent::AddMotion(std::string name, const char* filename)
{
	if (!(mMeshId == MeshID::kSkinnedMesh)) return false;

	return std::static_pointer_cast<SkinnedMesh>(m_pMesh)->AddMotion(name, filename);
}

//----------------------------------------------------------------------------------------------------------------------------

void MeshComponent::Play(std::string name)
{
	std::static_pointer_cast<SkinnedMesh>(m_pMesh)->Play(name);
}

//----------------------------------------------------------------------------------------------------------------------------

void MeshComponent::SetAdditionalResource(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context)
{
	if (m_pShaderResources.empty()) return;

	int i = 1;
	for (auto& texture : m_pShaderResources)
	{
		texture->Set(imm_context, i++);
	}
}

//----------------------------------------------------------------------------------------------------------------------------

void MeshComponent::SetTopology(D3D11_PRIMITIVE_TOPOLOGY type)
{
	m_pMesh->SetTopology(type);
}

//----------------------------------------------------------------------------------------------------------------------------

