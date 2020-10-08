#pragma once
#include "ColliderComponent.h"

class MoveComponent;

class SphereColliderComponent final : public ColliderComponent
{
public:
	float mRadius = 0.5f;
	std::shared_ptr<MoveComponent> m_pMover = nullptr;
private:
	SphereColliderComponent(const std::shared_ptr<Actor>& owner) :ColliderComponent(owner) {}

public:
	static std::shared_ptr<SphereColliderComponent> Initialize(const std::shared_ptr<Actor> owner);
	virtual bool Create();
	virtual void Destroy();

	virtual void Update(float elapsed_time);
	void ResponceToEnvironment(const Vector3& normal, float restitution, float penetration);
	void ResponceToSphere(const std::shared_ptr<SphereColliderComponent>& other, float restitution, float penetration);

	virtual void Intersect(const std::shared_ptr<ColliderComponent>& other) override;
	virtual void IntersectToSphere(const std::shared_ptr<ColliderComponent>& other) override;
	virtual void IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other) override;
	void IntersectToEnvironment(float restitution);

	static constexpr int GetID() { return ComponentID::kCollider; }
	inline float GetRadius() { return mRadius; }
	

	virtual ~SphereColliderComponent() = default;
};