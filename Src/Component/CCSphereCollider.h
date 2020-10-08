#pragma once
#include "ColliderComponent.h"

class MoveComponent;
class CCCapsuleCollider;

class CCSphereCollider : public ColliderComponent
{
private:
	float mRadius = 0.5f;
	std::shared_ptr<MoveComponent> m_pMover = nullptr;
	bool m_bOnIntersection = false;

private:
	CCSphereCollider(const std::shared_ptr<Actor>& other) :ColliderComponent(other) {}

public:
	static std::shared_ptr<CCSphereCollider> Initialize(const std::shared_ptr<Actor>& owner);

	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;

	virtual void Intersect(const std::shared_ptr<ColliderComponent>& other) override;
	virtual void IntersectToSphere(const std::shared_ptr<ColliderComponent>& other) override;
	virtual void IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other) override;
	void IntersectToPlanet(const std::shared_ptr<ColliderComponent>& other);

	void RespondToSphere(const std::shared_ptr<CCSphereCollider>& other, float restitution, float penetration);
	bool OnIntersection();

	static constexpr int GetID() { return ComponentID::kCollider; }
	float GetRadius() { return mRadius; }

	void SetOnIntersection(bool flag);
	void ResetColliderType(ColliderType id);
	void ResetRadius(float radius) { mRadius = radius; }

	virtual ~CCSphereCollider() = default;

	CCSphereCollider(const CCSphereCollider& other) = delete;
	CCSphereCollider& operator=(const CCSphereCollider& other) = delete;
};

inline bool CCSphereCollider::OnIntersection()
{
	return m_bOnIntersection;
}

inline void CCSphereCollider::SetOnIntersection(bool flag)
{
	m_bOnIntersection = flag;
}

inline void CCSphereCollider::ResetColliderType(ColliderType id)
{
	mType = id;
}