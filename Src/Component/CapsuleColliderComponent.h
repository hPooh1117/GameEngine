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
	MoveComponent* mpMover = nullptr;
	Vector3 mOffset = {};
	
public:
	CapsuleColliderComponent(Actor* owner);
	~CapsuleColliderComponent();

	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time);
	void ResponceToEnvironment(const Vector3& normal, float restitution, float penetration);
	void ResponceToCapsule(CapsuleColliderComponent* other, float restitution, float penetration);
	void ResponceToSphere(SphereColliderComponent* other, float restitution, float penetration);

	virtual void Intersect(ColliderComponent* other);
	virtual void IntersectToSphere(SphereColliderComponent* other);
	virtual void IntersectToCapsule(CapsuleColliderComponent* other);

	static constexpr int GetID() { return ComponentID::kCollider; }
	static constexpr int GetColliderType() { return ColliderType::kC_Capsule; }
	inline Segment& GetCenterSegment() { return mCenterSegment; }

	inline void SetOffset(Vector3& offset) { mOffset = offset; }
	inline void SetScale(Vector3& scale) { mScale = scale; }
};