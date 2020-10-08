#pragma once
#include "ColliderComponent.h"
#include "./Utilities/PrimitiveData.h"

class CCSphereCollider;
class MoveComponent;

class CCCapsuleCollider : public ColliderComponent
{
private:
	float mRadius = 0.5f;
	Segment mCenterSegment;
	std::shared_ptr<MoveComponent> m_pMover = nullptr;
	Vector3 mOffset = {};
	bool m_bOnIntersection = false;


private:
	CCCapsuleCollider(const std::shared_ptr<Actor>& owner) :ColliderComponent(owner) {}

public:
	static std::shared_ptr<CCCapsuleCollider> Initialize(const std::shared_ptr<Actor>& owner);

	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;

	virtual void Intersect(const std::shared_ptr<ColliderComponent>& other);

	void ResponceToEnvironment(const Vector3& normal, float restitution, float penetration);
	void ResponceToCapsule(const std::shared_ptr<CCCapsuleCollider>& other, float restitution, float penetration);
	void ResponceToSphere(const std::shared_ptr<CCSphereCollider>& other, float restitution, float penetration);

	virtual void IntersectToSphere(const std::shared_ptr<ColliderComponent>& other);
	virtual void IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other);
	inline bool OnIntersection();

	static constexpr int GetID() { return ComponentID::kCollider; }
	static constexpr int GetColliderType() { return ColliderType::kC_Capsule; }
	Segment& GetCenterSegment() { return mCenterSegment; }
	float GetRadius();

	void SetOffset(const Vector3& offset) { mOffset = offset; }
	void SetScale(const Vector3& scale) { mScale = scale; }

	virtual ~CCCapsuleCollider() = default;

	CCCapsuleCollider(const CCCapsuleCollider& other) = delete;
	CCCapsuleCollider& operator=(const CCCapsuleCollider& other) = delete;
};


inline bool CCCapsuleCollider::OnIntersection()
{
	return m_bOnIntersection;
}

inline float CCCapsuleCollider::GetRadius()
{
	return mRadius;
}