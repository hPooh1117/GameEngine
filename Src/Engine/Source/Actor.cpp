#include "Actor.h"

#include "./Component/MeshComponent.h"
#include "MeshRenderer.h"
//-----------------------------------------------------
// NS
//-----------------------------------------------------
using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

Actor::Actor(int id):mId(id)
{
    XMMATRIX r = XMMatrixIdentity();
    XMStoreFloat4x4(&mRotation, r);
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<Actor> Actor::Initialize(int id)
{
    return std::shared_ptr<Actor>(new Actor(id));
}

//----------------------------------------------------------------------------------------------------------------------------

Actor::~Actor()
{
};

//----------------------------------------------------------------------------------------------------------------------------

void Actor::Update(float elapsed_time)
{
    for (auto& component : m_pComponents)
    {
        component.second->Update(elapsed_time);
    }
}

//----------------------------------------------------------------------------------------------------------------------------

//std::shared_ptr<MeshComponent> Actor::AddComponent(
//	int mesh_id,
//	std::shared_ptr<MeshRenderer>& renderer,
//	std::shared_ptr<Shader>& shader
//)
//{
//	auto component = MeshComponent::Initialize(shared_from_this());
//	component->Create();
//	if (component->Load(mesh_id, renderer))
//	{
//#ifdef _DEBUG
//		Log::Info("[ACTOR] Complete Connecting Mesh with MeshComponent.");
//#endif
//	}
//
//	component->SetShader(shader);
//	m_pComponents.emplace(MeshComponent::GetID(), component);
//	return component;
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//std::shared_ptr<MeshComponent> Actor::AddComponent(
//	int mesh_id,
//	std::shared_ptr<MeshRenderer>& renderer,
//	std::shared_ptr<Shader>& shader,
//	const char* filename)
//{
//	auto component = MeshComponent::Initialize(shared_from_this());
//	component->Create();
//	if (component->Load(filename, mesh_id, renderer))
//	{
//#ifdef _DEBUG
//		Log::Info("[ACTOR] Complete Connecting Mesh with MeshComponent.");
//#endif
//	}
//	component->SetShader(shader);
//	m_pComponents.emplace(MeshComponent::GetID(), component);
//	return component;
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//std::shared_ptr<MeshComponent> Actor::AddComponent(
//	int mesh_id,
//	std::shared_ptr<MeshRenderer>& renderer,
//	std::shared_ptr<Shader>& shader,
//	const wchar_t* filename)
//{
//	auto component = MeshComponent::Initialize(shared_from_this());
//	component->Create();
//	if (component->Load(filename, mesh_id, renderer))
//	{
//#ifdef _DEBUG
//		Log::Info("[ACTOR] Complete Connecting Mesh with MeshComponent.");
//#endif
//	}
//	component->SetShader(shader);
//	m_pComponents.emplace(MeshComponent::GetID(), component);
//	return component;
//}
//
////----------------------------------------------------------------------------------------------------------------------------
//
//
//std::shared_ptr<MeshComponent> Actor::GetComponent(int no)
//{
//	auto range = m_pComponents.equal_range(kRender);
//
//	for (auto it = range.first; it != range.second; ++it)
//	{
//		auto target = *it;
//		if (no == std::static_pointer_cast<MeshComponent>(target.second)->GetMeshID())
//		{
//			std::shared_ptr<MeshComponent> component = std::static_pointer_cast<MeshComponent>(target.second);
//			return component;
//		}
//	}
//	return nullptr;
//}

//----------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX Actor::GetWorldMatrix()
{
	mRotation = Matrix::CreateFromQuaternion(mRot);

	DirectX::XMMATRIX additive = DirectX::XMMatrixIdentity();
	if (mAdditiveRotation != Vector3(0, 0, 0))
		additive = DirectX::XMMatrixRotationRollPitchYaw(mAdditiveRotation.x, mAdditiveRotation.y, mAdditiveRotation.z);

	DirectX::XMMATRIX world;
	world = DirectX::XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	world *= DirectX::XMLoadFloat4x4(&mRotation) * additive;
	world *= DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	return world;
}


//----------------------------------------------------------------------------------------------------------------------------
