#include "CCSphereCollider.h"

#include "MoveComponent.h"
#include "CCCapsuleCollider.h"
#include "CCPlanetResident.h"

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<CCSphereCollider> CCSphereCollider::Initialize(const std::shared_ptr<Actor>& owner)
{
    return std::shared_ptr<CCSphereCollider>(new CCSphereCollider(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool CCSphereCollider::Create()
{
    mType = ColliderType::kC_Sphere;
    m_pMover = OwnerPtr->GetComponent<MoveComponent>();
    mRadius = mRadius * m_pOwner.lock()->GetScaleValue();

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void CCSphereCollider::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void CCSphereCollider::Update(float elapsed_time)
{

    mPosition = OwnerPtr->GetPosition();
}

//----------------------------------------------------------------------------------------------------------------------------

void CCSphereCollider::Intersect(const std::shared_ptr<ColliderComponent>& other)
{
	int type = other->GetType();

	switch (type)
	{
	case kC_Sphere:
		IntersectToSphere(other);
		break;

	case kC_Capsule:
		IntersectToCapsule(other);
		break;

	case kC_Planet:
		IntersectToPlanet(other);
		break;
	default:
		break;
	}

}

//----------------------------------------------------------------------------------------------------------------------------

void CCSphereCollider::IntersectToSphere(const std::shared_ptr<ColliderComponent>& other)
{
	Vector3 e = mPosition - other->GetCenterPosition();
	float eLength = e.getLength();


	if (eLength < mRadius + std::static_pointer_cast<CCSphereCollider>(other)->GetRadius())
	{

		RespondToSphere(
			std::static_pointer_cast<CCSphereCollider>(other),
			0.6f,
			mRadius + std::static_pointer_cast<CCSphereCollider>(other)->GetRadius() - eLength
		);

	}

}

//----------------------------------------------------------------------------------------------------------------------------

void CCSphereCollider::IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other)
{
	Vector3 sphereCenter = mPosition;
	std::shared_ptr<CCCapsuleCollider> cap = std::static_pointer_cast<CCCapsuleCollider>(other);
	Segment capsuleCenter = cap->GetCenterSegment();
	Vector3 h;
	float t = 0.0f;

	float d = CalculateBetweenPointAndSegment(sphereCenter, capsuleCenter, h, t);

	m_bOnIntersection = false; // 毎フレームクリア

	float distance = mRadius + cap->GetRadius();

	if (d <= distance)
	{
		cap->ResponceToSphere(
			std::static_pointer_cast<CCSphereCollider>(shared_from_this()),
			0.9f,
			distance - d
		);
	}

}

//----------------------------------------------------------------------------------------------------------------------------

void CCSphereCollider::IntersectToPlanet(const std::shared_ptr<ColliderComponent>& other)
{
	Vector3 e = mPosition - other->GetCenterPosition();
	float eLength = e.getLength();


	if (eLength < mRadius + std::static_pointer_cast<CCSphereCollider>(other)->GetRadius())
	{

		RespondToSphere(
			std::static_pointer_cast<CCSphereCollider>(other),
			0.9f,
			mRadius + std::static_pointer_cast<CCSphereCollider>(other)->GetRadius() - eLength
		);
		m_pMover->BeOnGround();
		return;
	}

	m_pMover->BeOffGround();


}

//----------------------------------------------------------------------------------------------------------------------------

void CCSphereCollider::RespondToSphere(const std::shared_ptr<CCSphereCollider>& other, float restitution, float penetration)
{
	Vector3 pos1 = mPosition;
	Vector3 pos2 = other->mPosition;
	Vector3 n = pos2 - pos1;
	n.normalize();

	float m1 = m_pMover->GetMass();
	float m2 = other->m_pMover->GetMass();

	float v1 = m_pMover->GetVelocity().dot(n);
	float v2 = other->m_pMover->GetVelocity().dot(n);

	if (v1 - v2 > 0)
	{
		float _v1 = (m1 * v1 + m2 * v2 + restitution * m2 * (v2 - v1)) / (m1 + m2);
		float _v2 = (m1 * v1 + m2 * v2 + restitution * m1 * (v1 - v2)) / (m1 + m2);

		m_pMover->SetVelocity(m_pMover->GetVelocity() + (_v1 - v1) * n);
		other->m_pMover->SetVelocity(other->m_pMover->GetVelocity() + (_v2 - v2) * n);
	}



	m_pOwner.lock()->SetPosition(pos1 - (m2 / (m1 + m2) * penetration * n));
	other->m_pOwner.lock()->SetPosition(pos2 + (m1 / (m1 + m2) * penetration * n));

}

//----------------------------------------------------------------------------------------------------------------------------

