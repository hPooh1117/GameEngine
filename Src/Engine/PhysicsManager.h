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
	static constexpr float RESTITUTION_FACTOR = 0.8f;
	int mNumberOfSphere = 0;
	std::vector<std::shared_ptr < ObjectForTree<SphereColliderComponent>>> m_pObjTrees;
	std::unique_ptr< LinearQuadTree<SphereColliderComponent>> m_pQuadTree;
	std::vector<std::shared_ptr<SphereColliderComponent>> mCollisionList;
	int mCollisionCounter = 0;
public:
	PhysicsManager();

	bool Init(const std::unique_ptr<ActorManager>& actor_manager, bool b_used_octree = false);
	void RegistSphere(const std::unique_ptr<ActorManager>& actor_manager);

	void DetectCollision(const std::unique_ptr<ActorManager>& actor_manager);
	void DetectCollision(const std::unique_ptr<ActorManager>& actor_manager, float elapsed_time);
	void CollisionToEnvironment(std::shared_ptr<Actor>& actor);
	//void CollisionToEnvirionment(SphereColliderComponent* sphere);

	void GetNextPosition(const std::shared_ptr<SphereColliderComponent>& sphere, float elapsed_time);
	inline int GetCollisionCounter() { return mCollisionCounter; }
	~PhysicsManager();
};