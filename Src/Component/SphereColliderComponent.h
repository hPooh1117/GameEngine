#pragma once
#include "ColliderComponent.h"

class MoveComponent;

class SphereColliderComponent final : public ColliderComponent
{
public:
	float mRadius = 0.5f;
	MoveComponent* m_pMover = nullptr;

public:
	SphereColliderComponent(Actor* owner) :ColliderComponent(owner) {}
	virtual ~SphereColliderComponent() = default;


	virtual bool Create();
	virtual void Destroy();

	virtual void Update(float elapsed_time);

	void		 ResponceToEnvironment(const Vector3& normal, float restitution, float penetration);
	void		 ResponceToSphere(SphereColliderComponent* other, float restitution, float penetration);

	virtual void Intersect(ColliderComponent* other) override;
	virtual void IntersectToSphere(SphereColliderComponent* other) override;
	virtual void IntersectToCapsule(CapsuleColliderComponent* other) override;
	void         IntersectToEnvironment(float restitution);

	static constexpr int GetID() { return ComponentID::kCollider; }
	inline float         GetRadius() { return mRadius; }
};