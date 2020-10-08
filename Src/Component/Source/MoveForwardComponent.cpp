#include "MoveForwardComponent.h"

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

MoveForwardComponent::MoveForwardComponent(const std::shared_ptr<Actor>& owner):MoveComponent(owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

MoveForwardComponent::~MoveForwardComponent()
{
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<MoveForwardComponent> MoveForwardComponent::Initialize(const std::shared_ptr<Actor>& owner)
{
	return std::shared_ptr<MoveForwardComponent>(new MoveForwardComponent(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool MoveForwardComponent::Create()
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void MoveForwardComponent::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void MoveForwardComponent::Update(float elapsed_time)
{
	if (mIsValid == false) return;
	//XMVECTOR rotation = m_owner->getRotation();
	//XMMATRIX m = XMLoadFloat4x4(&(m_owner->getRotationMatrix()));
	Matrix R = Matrix::CreateFromQuaternion(m_pOwner.lock()->GetQuaternion());
	//XMStoreFloat4x4(&R, m);

	mVelocity = mSpeed * Vector3(R._31, R._32, R._33);
	Vector3 displacement = mVelocity * elapsed_time;
	m_pOwner.lock()->SetPosition(m_pOwner.lock()->GetPosition() + displacement);

}

//----------------------------------------------------------------------------------------------------------------------------
