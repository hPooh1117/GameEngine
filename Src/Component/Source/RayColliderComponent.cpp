#include "RayColliderComponent.h"
#include "MeshComponent.h"

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

RayColliderComponent::RayColliderComponent(const std::shared_ptr<Actor>& owner):ColliderComponent(owner)
{
	mPosition = m_pOwner.lock()->GetPosition();
	float rz = -90.0f;
	XMStoreFloat4x4(&mOrientation, XMMatrixRotationRollPitchYaw(0, 0, rz * 0.01745f));
	mScale = Vector3(3, 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<RayColliderComponent> RayColliderComponent::Initialize(const std::shared_ptr<Actor>& owner)
{
	return std::shared_ptr<RayColliderComponent>(new RayColliderComponent(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool RayColliderComponent::Create()
{
	std::shared_ptr<MeshComponent> rayMesh = m_pOwner.lock()->GetComponent(MeshComponent::MeshID::EBasicLine);
	assert(rayMesh != nullptr);


	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void RayColliderComponent::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void RayColliderComponent::Update(float elapsed_time)
{
	mPosition = m_pOwner.lock()->GetPosition();
}

//----------------------------------------------------------------------------------------------------------------------------

void RayColliderComponent::Intersect(const std::shared_ptr<ColliderComponent>& other)
{
}

//----------------------------------------------------------------------------------------------------------------------------

void RayColliderComponent::IntersectToSphere(const std::shared_ptr<ColliderComponent>& other)
{
}

//----------------------------------------------------------------------------------------------------------------------------

void RayColliderComponent::IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other)
{
}

//----------------------------------------------------------------------------------------------------------------------------

RayColliderComponent::~RayColliderComponent()
{
}

//----------------------------------------------------------------------------------------------------------------------------
