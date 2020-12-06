#include "CCPlayerMove.h"

#include "./Application/Input.h"

#include "./Engine/Actor.h"

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

CCPlayerMove::CCPlayerMove(Actor* owner):MoveComponent(owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

bool CCPlayerMove::Create()
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerMove::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerMove::Update(float elapsed_time)
{
	if (mIsValid == false) return;

	Vector3 drag = -mVelocity * 0.5f;
	AddForce(drag);

	MovePlayer();

	AddForce(mMass * G);

	Integrate(elapsed_time);

}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerMove::MovePlayer()
{
	Matrix R = Matrix::CreateFromQuaternion(mpOwner->GetQuaternion());
	Vector3 forward = R.GetZAxis();
	Vector3 up = R.GetYAxis();
	Vector3 right = R.GetXAxis();

	mDisplacement = {};

	if (InputPtr.OnKeyDown("Up") || InputPtr.GetThumbLYValue() > 0.0f)
	{
		mDisplacement += forward;
		//AddForce(mMass * forward * mSpeed);
	}
	else if (InputPtr.OnKeyDown("Down") || InputPtr.GetThumbLYValue() < 0.0f)
	{
		mDisplacement += -forward;
		//AddForce(mMass * -forward * mSpeed);
	}
	if (InputPtr.OnKeyDown("Left") || InputPtr.GetThumbLXValue() < 0.0f)
	{
		mDisplacement += -right;
		//AddForce(mMass * -right * mSpeed);
	}
	else if (InputPtr.OnKeyDown("Right") || InputPtr.GetThumbLXValue() > 0.0f)
	{
		mDisplacement += right;
		//AddForce(mMass * right * mSpeed);
	}
	if (mDisplacement != Vector3(0, 0, 0)) mDisplacement = Vector3::Normalize(mDisplacement) * mSpeed;
	mpOwner->SetPosition(mpOwner->GetPosition() + mDisplacement);
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerMove::Integrate(float elapsed_time)
{
	assert(mMass > 0);

	mAcceleration = mResultant / mMass;
	mVelocity += mAcceleration * elapsed_time;

	mpOwner->SetPosition(mpOwner->GetPosition() + mVelocity * elapsed_time);

	mResultant = {};
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerMove::AddForce(const Vector3& force)
{
	mResultant += force;
}

//----------------------------------------------------------------------------------------------------------------------------
