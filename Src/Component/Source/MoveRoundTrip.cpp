#include "MoveRoundTrip.h"

#include "./Engine/Actor.h"


//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

MoveRoundTrip::MoveRoundTrip(const std::shared_ptr<Actor>& owner) :MoveComponent(owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

MoveRoundTrip::~MoveRoundTrip()
{
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<MoveRoundTrip> MoveRoundTrip::Initialize(const std::shared_ptr<Actor>& owner)
{
	return std::shared_ptr<MoveRoundTrip>(new MoveRoundTrip(owner));
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
	Matrix R = Matrix::CreateFromQuaternion(m_pOwner.lock()->GetQuaternion());
	//XMStoreFloat4x4(&R, m);
	Vector3 pos = m_pOwner.lock()->GetPosition();

	if (pos.x < -5 || pos.x > 5) m_speed *= -1;

	mVelocity = m_speed * Vector3(R._11, R._12, R._13);
	Vector3 displacement = mVelocity * elapsed_time;
	m_pOwner.lock()->SetPosition(pos + displacement);

}

//----------------------------------------------------------------------------------------------------------------------------
