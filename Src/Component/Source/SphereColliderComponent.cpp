#include "SphereColliderComponent.h"
#include "MoveComponent.h"
#include "CapsuleColliderComponent.h"

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<SphereColliderComponent> SphereColliderComponent::Initialize(const std::shared_ptr<Actor> owner)
{
	return std::shared_ptr<SphereColliderComponent>(new SphereColliderComponent(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool SphereColliderComponent::Create()
{
	mType = ColliderType::kC_Sphere;
	m_pMover = m_pOwner.lock()->GetComponent<MoveComponent>();
	mRadius = mRadius * m_pOwner.lock()->GetScaleValue();

	if (m_pMover != nullptr) return true;
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------

void SphereColliderComponent::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SphereColliderComponent::Update(float elapsed_time)
{
	mPosition = m_pOwner.lock()->GetPosition();
}

//----------------------------------------------------------------------------------------------------------------------------

void SphereColliderComponent::ResponceToEnvironment(const Vector3& normal, float restitution, float penetration)
{
	Vector3& position = m_pOwner.lock()->GetPosition();
	Vector3& velocity = m_pMover->GetVelocity();
	float dot = velocity.dot(normal);
	if (dot < 0)
	{
		velocity += -(restitution + 1) * dot * normal;
	}
	position += normal * penetration;
}

//----------------------------------------------------------------------------------------------------------------------------

void SphereColliderComponent::ResponceToSphere(const std::shared_ptr<SphereColliderComponent>& other, float restitution, float penetration)
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

void SphereColliderComponent::Intersect(const std::shared_ptr<ColliderComponent>& other)
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
	default:
		break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------

void SphereColliderComponent::IntersectToSphere(const std::shared_ptr<ColliderComponent>& other)
{
	Vector3 e = mPosition - other->GetCenterPosition();
	float eLength = e.getLength();


	if (eLength < mRadius + std::static_pointer_cast<SphereColliderComponent>(other)->GetRadius())
	{

		ResponceToSphere(
			std::static_pointer_cast<SphereColliderComponent>(other),
			0.9f,
			mRadius + std::static_pointer_cast<SphereColliderComponent>(other)->GetRadius() - eLength
		);

	}

}

//----------------------------------------------------------------------------------------------------------------------------

void SphereColliderComponent::IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other)
{
	Vector3 sphereCenter = mPosition;
	std::shared_ptr<CapsuleColliderComponent> cap = std::static_pointer_cast<CapsuleColliderComponent>(other);
	Segment capsuleCenter = cap->GetCenterSegment();
	Vector3 h;
	float t = 0.0f;

	float d = CalculateBetweenPointAndSegment(sphereCenter, capsuleCenter, h, t);

	if (d <= 1.0f)
	{
		cap->ResponceToSphere(
			std::static_pointer_cast<SphereColliderComponent>(shared_from_this()),
			0.9f,
			1.0f - d
		);
		
	}

}

//----------------------------------------------------------------------------------------------------------------------------

void SphereColliderComponent::IntersectToEnvironment(float restitution)
{
	if (mPosition.x + mRadius > 10)
	{
		ResponceToEnvironment(Vector3(-1, 0, 0), restitution, (mRadius + mPosition.x) - 10);
	}
	else if (mPosition.x - mRadius < -10)
	{
		ResponceToEnvironment(Vector3(1, 0, 0), restitution, -10 - (mPosition.x - mRadius));
	}
	else if (mPosition.z + mRadius > 10)
	{
		ResponceToEnvironment(Vector3(0, 0, -1), restitution, (mRadius + mPosition.z) - 10);
	}
	else if (mPosition.z - mRadius < -10)
	{
		ResponceToEnvironment(Vector3(0, 0, 1), restitution, -10 - (mPosition.z - mRadius));
	}

	if (mPosition.y < mRadius)
	{
		ResponceToEnvironment(Vector3(0, 1, 0), restitution, mRadius - mPosition.y);
	}
}

//----------------------------------------------------------------------------------------------------------------------------

