#include "MoveRoundTrip.h"

#include "./Engine/Actor.h"


//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

MoveRoundTrip::MoveRoundTrip(Actor* owner) :MoveComponent(owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

bool MoveRoundTrip::Create()
{
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------

void MoveRoundTrip::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void MoveRoundTrip::Update(float elapsed_time)
{
	if (mIsValid == false) return;
	//XMVECTOR rotation = m_owner->getRotation();
	//XMMATRIX m = XMLoadFloat4x4(&(m_owner->getRotationMatrix()));
	Matrix R = Matrix::CreateFromQuaternion(mpOwner->GetQuaternion());
	//XMStoreFloat4x4(&R, m);
	Vector3 pos = mpOwner->GetPosition();

	if (pos.x < -5 || pos.x > 5) mSpeed *= -1;

	mVelocity = mSpeed * Vector3(R._11, R._12, R._13);
	Vector3 displacement = mVelocity * elapsed_time;
	mpOwner->SetPosition(pos + displacement);

}

//----------------------------------------------------------------------------------------------------------------------------
