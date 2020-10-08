#pragma once
#include "ColliderComponent.h"

class RayColliderComponent : public ColliderComponent
{
private:

private:
	RayColliderComponent(const std::shared_ptr<Actor>& owner);

public:
	static std::shared_ptr<RayColliderComponent> Initialize(const std::shared_ptr<Actor>& owner);
	virtual bool Create();
	virtual void Destroy();

	virtual void Update(float elapsed_time);

	virtual void Intersect(const std::shared_ptr<ColliderComponent>& other);
	virtual void IntersectToSphere(const std::shared_ptr<ColliderComponent>& other);
	virtual void IntersectToCapsule(const std::shared_ptr<ColliderComponent>& other);
	static constexpr int GetID() { return ComponentID::kCollider; }


	~RayColliderComponent();
};