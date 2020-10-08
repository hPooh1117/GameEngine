#pragma once
#include "Component.h"

#include <DirectXMath.h>
#include <memory>

#include "./Utilities/Vector.h"
#include "./Utilities/PrimitiveData.h"

class Actor;

enum ColliderType
{
	kC_None,
	kC_Sphere,
	kC_AABB,
	kC_Capsule,
	kC_CCCapsule,
	kC_Planet,

	TYPE_MAX,
};

class ColliderComponent : public Component
{
protected:
	Vector3 mPosition;
	DirectX::XMFLOAT4X4 mOrientation = {};
	Vector3 mScale;


	ColliderType mType = ColliderType::kC_None;
protected:
	ColliderComponent(const std::shared_ptr<Actor>& owner) : Component(owner) {}

public:
	virtual ~ColliderComponent() {}

	virtual bool Create() = 0;
	virtual void Destroy() = 0;

	virtual void Update(float elapsed_time) = 0;

	virtual void Intersect(const std::shared_ptr<ColliderComponent>& other) = 0;
	virtual void IntersectToSphere(const std::shared_ptr<ColliderComponent>& other) = 0;
	virtual void IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other) = 0;

	// calculation of shortest distance
	float CalculateBetweenPointAndLine(
		const Vector3& p,
		const Line& l,
		Vector3& h,
		float& t
	);
	float CalculateBetweenLines(
		const Line& l11,
		const Line& l22,
		Vector3& h1,
		Vector3& h2,
		float& t1,
		float& t2
	);
	float CalculateBetweenPointAndSegment(
		const Vector3& p,
		const Segment& s,
		Vector3& h,
		float& t
	);

	float CalculateBetweenSegments(
		const Segment& s1,
		const Segment& s2,
		Vector3& h1,
		Vector3& h2,
		float t1,
		float t2
	);

	bool IsAcute(
		const Vector3& p1,
		const Vector3& p2,
		const Vector3& p3
	);



	static constexpr int GetID() { return ComponentID::kCollider; }
	inline ColliderType& GetType() { return mType; }

	inline Vector3& GetCenterPosition() { return mPosition; }

	void SetCenterPosition(Vector3& pos);

	DirectX::XMMATRIX GetWorldMatrix();
};

