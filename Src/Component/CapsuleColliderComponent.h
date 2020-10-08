#pragma once
#include "ColliderComponent.h"	
#include "./Utilities/PrimitiveData.h"

class MoveComponent;
class SphereColliderComponent;

class CapsuleColliderComponent : public ColliderComponent
{
private:
	float mRadius = 0.5f;
	Segment mCenterSegment;
	std::shared_ptr<MoveComponent> m_pMover = nullptr;
	Vector3 mOffset = {};
	
private:
	CapsuleColliderComponent(const std::shared_ptr<Actor>& owner);

public:
	static std::shared_ptr<CapsuleColliderComponent> Initialize(const std::shared_ptr<Actor>& owner);
	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time);
	void ResponceToEnvironment(const Vector3& normal, float restitution, float penetration);
	void ResponceToCapsule(std::shared_ptr<CapsuleColliderComponent> other, float restitution, float penetration);
	void ResponceToSphere(std::shared_ptr<SphereColliderComponent> other, float restitution, float penetration);

	virtual void Intersect(const std::shared_ptr<ColliderComponent>& other);
	virtual void IntersectToSphere(const std::shared_ptr<ColliderComponent>& other);
	virtual void IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other);

	static constexpr int GetID() { return ComponentID::kCollider; }
	static constexpr int GetColliderType() { return ColliderType::kC_Capsule; }
	inline Segment& GetCenterSegment() { return mCenterSegment; }

	inline void SetOffset(Vector3& offset) { mOffset = offset; }
	inline void SetScale(Vector3& scale) { mScale = scale; }
	~CapsuleColliderComponent();
};