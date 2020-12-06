#include "CCPhysicalMove.h"
#include "./Engine/Actor.h"

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

CCPhysicalMove::CCPhysicalMove(Actor* owner):MoveComponent(owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

bool CCPhysicalMove::Create()
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPhysicalMove::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPhysicalMove::Update(float elapsed_time)
{
	if (mIsValid == false) return;

	AddForce(mMass * G);

	Integrate(elapsed_time);
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPhysicalMove::Integrate(float elapsed_time)
{
	assert(mMass > 0);

	mAcceleration = mResultant / mMass;
	mVelocity += mAcceleration * elapsed_time;
	mpOwner->SetPosition(mpOwner->GetPosition() + mVelocity * elapsed_time);

	mResultant = {};
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPhysicalMove::AddForce(const Vector3& force)
{
	mResultant += force;
}

//----------------------------------------------------------------------------------------------------------------------------
