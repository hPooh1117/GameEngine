#pragma once
#include <memory>
#include <unordered_map>

#include "QuadTree.h"

#include "./Utilities/PrimitiveData.h"

class Actor;
class ActorManager;
class SphereColliderComponent;

class PhysicsManager final
{
private:
	static constexpr float RESTITUTION_FACTOR = 0.65f;
	int mNumberOfSphere = 0;
	std::vector<std::shared_ptr<ObjectForTree<SphereColliderComponent>>> m_pObjTrees;
	std::unique_ptr< LinearQuadTree<SphereColliderComponent>> m_pQuadTree;
	std::vector<SphereColliderComponent*> mCollisionList;
	int mCollisionCounter = 0;
public:
	PhysicsManager();
	~PhysicsManager();

	bool Init(ActorManager* actor_manager, bool b_used_octree = false);

	void DetectCollision(ActorManager* actor_manager);
	void DetectCollision(ActorManager* actor_manager, float elapsed_time);

private:
	void RegistSphere(ActorManager* actor_manager);
	void CollisionToEnvironment(Actor* actor);
	//void CollisionToEnvirionment(SphereColliderComponent* sphere);

public:
	void GetNextPosition(SphereColliderComponent* sphere, float elapsed_time);
	inline int GetCollisionCounter() { return mCollisionCounter; }
};