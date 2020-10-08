#include "CCCapsuleCollider.h"
#include "CCSphereCollider.h"
#include "MoveComponent.h"

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<CCCapsuleCollider> CCCapsuleCollider::Initialize(const std::shared_ptr<Actor>& owner)
{
    return std::shared_ptr<CCCapsuleCollider>(new CCCapsuleCollider(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool CCCapsuleCollider::Create()
{
	mType = ColliderType::kC_Capsule;
	m_pMover = m_pOwner.lock()->GetComponent<MoveComponent>();

	if (m_pMover != nullptr) return true;

    return false;
}

//----------------------------------------------------------------------------------------------------------------------------

void CCCapsuleCollider::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void CCCapsuleCollider::Update(float elapsed_time)
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

void CCCapsuleCollider::Intersect(const std::shared_ptr<ColliderComponent>& other)
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

void CCCapsuleCollider::ResponceToEnvironment(const Vector3& normal, float restitution, float penetration)
{
}

//----------------------------------------------------------------------------------------------------------------------------

void CCCapsuleCollider::ResponceToCapsule(const std::shared_ptr<CCCapsuleCollider>& other, float restitution, float penetration)
{
}

//----------------------------------------------------------------------------------------------------------------------------

void CCCapsuleCollider::ResponceToSphere(const std::shared_ptr<CCSphereCollider>& other, float restitution, float penetration)
{
	m_bOnIntersection = true;
	other->SetOnIntersection(true);
}
//----------------------------------------------------------------------------------------------------------------------------

void CCCapsuleCollider::IntersectToSphere(const std::shared_ptr<ColliderComponent>& other)
{
	std::static_pointer_cast<CCSphereCollider>(other)->SetOnIntersection(false); // 毎フレームクリア

	Vector3 sphereCenter = other->GetCenterPosition();
	Vector3 h;
	float t = 0.0f;

	float d = CalculateBetweenPointAndSegment(sphereCenter, mCenterSegment, h, t);
	float distance = mRadius * mScale.x + std::static_pointer_cast<CCSphereCollider>(other)->GetRadius();


	if (d <= distance)
	{
		ResponceToSphere(
			std::static_pointer_cast<CCSphereCollider>(other),
			0.9f,
			distance - d
		);
	}

}

//----------------------------------------------------------------------------------------------------------------------------

void CCCapsuleCollider::IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other)
{
}

//----------------------------------------------------------------------------------------------------------------------------
