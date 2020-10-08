#include "CapsuleColliderComponent.h"
#include "MoveComponent.h"
#include "SphereColliderComponent.h"

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

CapsuleColliderComponent::CapsuleColliderComponent(const std::shared_ptr<Actor>& owner):ColliderComponent(owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<CapsuleColliderComponent> CapsuleColliderComponent::Initialize(const std::shared_ptr<Actor>& owner)
{
	return std::shared_ptr<CapsuleColliderComponent>(new CapsuleColliderComponent(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool CapsuleColliderComponent::Create()
{

	mType = ColliderType::kC_Capsule;
	m_pMover = m_pOwner.lock()->GetComponent<MoveComponent>();
	mScale = m_pOwner.lock()->GetScale();
	if (m_pMover != nullptr) return true;
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------

void CapsuleColliderComponent::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void CapsuleColliderComponent::Update(float elapsed_time)
{

	float scaleY = mScale.y;

	mPosition = m_pOwner.lock()->GetPosition() + mOffset;

	Matrix rotation = Matrix::CreateFromQuaternion(m_pOwner.lock()->GetQuaternion());
	DirectX::XMMATRIX rot = DirectX::XMLoadFloat4x4(&rotation);

	Vector3 dirStart(0, -1 * scaleY * 0.5f, 0);
	Vector3 dirEnd(0, 1 * scaleY * 0.5f, 0);
	
	DirectX::XMVECTOR dS = DirectX::XMLoadFloat3(&(dirStart));
	DirectX::XMVECTOR dE = DirectX::XMLoadFloat3(&(dirEnd));

	dS = DirectX::XMVector3Transform(dS, rot);
	dE = DirectX::XMVector3Transform(dE, rot);
	
	DirectX::XMStoreFloat3(&dirStart, dS);
	DirectX::XMStoreFloat3(&dirEnd, dE);
	
	mCenterSegment.start = mPosition + Vector3(dirStart);
	mCenterSegment.end = mPosition + Vector3(dirEnd);
}

//----------------------------------------------------------------------------------------------------------------------------

void CapsuleColliderComponent::ResponceToEnvironment(const Vector3& normal, float restitution, float penetration)
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

void CapsuleColliderComponent::ResponceToCapsule(std::shared_ptr<CapsuleColliderComponent> other, float restitution, float penetration)
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

void CapsuleColliderComponent::ResponceToSphere(
	std::shared_ptr<SphereColliderComponent> other, 
	float restitution,
	float penetration)
{
	//Vector3 pos1 = mPosition;
	//Vector3 pos2 = other->GetCenterPosition();
	//Vector3 n = pos2 - pos1;
	//n.normalize();

	//float m1 = m_pMover->GetMass();
	//float m2 = other->m_pMover->GetMass();

	//float v1 = m_pMover->GetVelocity().dot(n);
	//float v2 = other->m_pMover->GetVelocity().dot(n);

	//if (v1 - v2 > 0)
	//{
	//	float _v1 = (m1 * v1 + m2 * v2 + restitution * m2 * (v2 - v1)) / (m1 + m2);
	//	float _v2 = (m1 * v1 + m2 * v2 + restitution * m1 * (v1 - v2)) / (m1 + m2);

	//	m_pMover->SetVelocity(m_pMover->GetVelocity() + (_v1 - v1) * n);
	//	other->m_pMover->SetVelocity(other->m_pMover->GetVelocity() + (_v2 - v2) * n);
	//}

	//m_pOwner.lock()->SetPosition(pos1 - (m2 / (m1 + m2) * penetration * n));
	//other->SetCenterPosition(pos2 + (m1 / (m1 + m2) * penetration * n));

	
}

//----------------------------------------------------------------------------------------------------------------------------

void CapsuleColliderComponent::Intersect(const std::shared_ptr<ColliderComponent>& other)
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

void CapsuleColliderComponent::IntersectToSphere(const std::shared_ptr<ColliderComponent>& other)
{
	Vector3 sphereCenter = other->GetCenterPosition();
	Vector3 h;
	float t = 0.0f;

	float d = CalculateBetweenPointAndSegment(sphereCenter, mCenterSegment, h, t);

	

	if (d <= 1.0f)
	{
		ResponceToSphere(
			std::static_pointer_cast<SphereColliderComponent>(other),
			0.9f,
			1.0f - d
		);
	}

}

//----------------------------------------------------------------------------------------------------------------------------

void CapsuleColliderComponent::IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other)
{
	std::shared_ptr<CapsuleColliderComponent> capsule = std::static_pointer_cast<CapsuleColliderComponent>(other);
	Segment s1 = mCenterSegment;
	Segment s2 = capsule->GetCenterSegment();
	Vector3 h1, h2;
	float t1 = 0.0f, t2 = 0.0f;
	float d = CalculateBetweenSegments(s1, s2, h1, h2, t1, t2);

	

	if (d <= mRadius + mRadius)
	{
		ResponceToCapsule(
			capsule,
			0.9f,
			1.0f - d
		);
	}

}

//----------------------------------------------------------------------------------------------------------------------------

CapsuleColliderComponent::~CapsuleColliderComponent()
{
}

//----------------------------------------------------------------------------------------------------------------------------
