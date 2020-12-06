#include "MoveForwardComponent.h"

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

MoveForwardComponent::MoveForwardComponent(Actor* owner):MoveComponent(owner)
{
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
	Matrix R = Matrix::CreateFromQuaternion(mpOwner->GetQuaternion());
	//XMStoreFloat4x4(&R, m);

	mVelocity = mSpeed * Vector3(R._31, R._32, R._33);
	Vector3 displacement = mVelocity * elapsed_time;
	mpOwner->SetPosition(mpOwner->GetPosition() + displacement);

}

//----------------------------------------------------------------------------------------------------------------------------
