#include "CCPhysicalMove.h"
#include "./Engine/Actor.h"

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

CCPhysicalMove::CCPhysicalMove(const std::shared_ptr<Actor>& owner):MoveComponent(owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<CCPhysicalMove> CCPhysicalMove::Initialize(const std::shared_ptr<Actor>& owner)
{
	return std::shared_ptr<CCPhysicalMove>(new CCPhysicalMove(owner));
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
	m_pOwner.lock()->SetPosition(m_pOwner.lock()->GetPosition() + mVelocity * elapsed_time);

	mResultant = {};
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPhysicalMove::AddForce(const Vector3& force)
{
	mResultant += force;
}

//----------------------------------------------------------------------------------------------------------------------------

CCPhysicalMove::~CCPhysicalMove()
{
}

//----------------------------------------------------------------------------------------------------------------------------
