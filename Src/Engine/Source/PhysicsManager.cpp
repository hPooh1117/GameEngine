#include "PhysicsManager.h"
#include "Actor.h"
#include "ActorManager.h"
#include <iostream>
#include "./Component/SphereColliderComponent.h"
#include "./Component/CapsuleColliderComponent.h"

#include "./Utilities/Log.h"

//--------------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;

//--------------------------------------------------------------------------------------------------------------------------------

PhysicsManager::PhysicsManager()
{
    
}

//--------------------------------------------------------------------------------------------------------------------------------

bool PhysicsManager::Init(ActorManager* actor_manager, bool b_used_octree)
{
    Log::Info("[PHYSICS] Initializing PhysicsManager.");
    if (b_used_octree)
    {
        RegistSphere(actor_manager);

        m_pQuadTree = std::make_unique<LinearQuadTree<SphereColliderComponent>>();
        if (!m_pQuadTree->Init(4, Vector3(-100.0f, 50.0f, -100.0f), Vector3(100.0f, -300.0f, 100.0f)))
        {
            std::cout << "not create quad tree" << std::endl;
            return false;
        }

    }
    Log::Info("[PHYSICS] Initialized.");

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void PhysicsManager::RegistSphere(ActorManager* actor_manager)
{
    //mNumberOfSphere += add;

    for (auto it = actor_manager->GetActors().begin(); it != actor_manager->GetActors().end(); ++it)
    {
        SphereColliderComponent* sphere = it->second->GetComponent<SphereColliderComponent>();
        if (sphere == nullptr) continue;
        if (sphere->GetType() != kC_Sphere) continue;

        m_pObjTrees.emplace_back(std::make_shared< ObjectForTree<SphereColliderComponent>>());

        m_pObjTrees.at(mNumberOfSphere)->m_pObject = sphere;
        mNumberOfSphere++;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

void PhysicsManager::DetectCollision(ActorManager* actor_manager)
{
    for (unsigned int i = 0; i < actor_manager->GetActorsSize(); ++i)
{
    Actor* actor1 = actor_manager->GetActor(i);
    ColliderComponent* collider1 = actor1->GetComponent<ColliderComponent>();
    if (collider1 == nullptr) continue;


    for (unsigned int j = i + 1; j < actor_manager->GetActorsSize(); ++j)
    {
        Actor* actor2 = actor_manager->GetActor(j);

        ColliderComponent* collider2 = actor2->GetComponent<ColliderComponent>();
        if (collider2 == nullptr) continue;


        collider1->Intersect(collider2);
    }

    //CollisionToEnvironment(actor1);
}

}

//--------------------------------------------------------------------------------------------------------------------------------

void PhysicsManager::DetectCollision(ActorManager* actor_manager, float elapsed_time)
{
    if (m_pQuadTree == nullptr) return;

    for (int i = 0; i < mNumberOfSphere; ++i)
    {
        SphereColliderComponent* pTmp = m_pObjTrees.at(i)->m_pObject;
        
        GetNextPosition(pTmp, elapsed_time);
        m_pObjTrees[i]->Remove();

        float radius = pTmp->GetRadius();
        Vector3 tmpMin = pTmp->GetCenterPosition() - Vector3(radius, radius, radius);
        Vector3 tmpMax = pTmp->GetCenterPosition() + Vector3(radius, radius, radius);
        m_pQuadTree->Regist(&tmpMin, &tmpMax, m_pObjTrees.at(i));
    }

    mCollisionCounter = m_pQuadTree->GetAllCollisionList(mCollisionList);

    for (auto i = 0; i < mCollisionCounter / 2; ++i)
    {
        mCollisionList[i * 2]->Intersect(mCollisionList[i * 2 + 1]);
    }

}

//--------------------------------------------------------------------------------------------------------------------------------

void PhysicsManager::CollisionToEnvironment(Actor* actor)
{
    int type = actor->GetComponent<ColliderComponent>()->GetType();

    if (type == ColliderType::kC_Sphere)
    {
        SphereColliderComponent* collider = actor->GetComponent<SphereColliderComponent>();

        if (actor->GetPosition().x + 0.5f > 10)
        {
            collider->ResponceToEnvironment(Vector3(-1, 0, 0), RESTITUTION_FACTOR, (0.5f + actor->GetPosition().x) - 10);
        }
        else if (actor->GetPosition().x - 0.5f < -10)
        {
            collider->ResponceToEnvironment(Vector3(1, 0, 0), RESTITUTION_FACTOR, -10 - (actor->GetPosition().x - 0.5f));
        }
        else if (actor->GetPosition().z + 0.5f > 10)
        {
            collider->ResponceToEnvironment(Vector3(0, 0, -1), RESTITUTION_FACTOR, (0.5f + actor->GetPosition().z) - 10);
        }
        else if (actor->GetPosition().z - 0.5f < -10)
        {
            collider->ResponceToEnvironment(Vector3(0, 0, 1), RESTITUTION_FACTOR, -10 - (actor->GetPosition().z - 0.5f));
        }

        if (actor->GetPosition().y < 1.0f)
        {
            collider->ResponceToEnvironment(Vector3(0, 1, 0), RESTITUTION_FACTOR, 1.0f - actor->GetPosition().y);
        }
    }

    if (type == ColliderType::kC_Capsule)
    {
        CapsuleColliderComponent* collider = actor->GetComponent<CapsuleColliderComponent>();

        if (actor->GetPosition().x + 0.5f > 10)
        {
            collider->ResponceToEnvironment(Vector3(-1, 0, 0), RESTITUTION_FACTOR, (0.5f + actor->GetPosition().x) - 10);
        }
        else if (actor->GetPosition().x - 0.5f < -10)
        {
            collider->ResponceToEnvironment(Vector3(1, 0, 0), RESTITUTION_FACTOR, -10 - (actor->GetPosition().x - 0.5f));
        }
        else if (actor->GetPosition().z + 0.5f > 10)
        {
            collider->ResponceToEnvironment(Vector3(0, 0, -1), RESTITUTION_FACTOR, (0.5f + actor->GetPosition().z) - 10);
        }
        else if (actor->GetPosition().z - 0.5f < -10)
        {
            collider->ResponceToEnvironment(Vector3(0, 0, 1), RESTITUTION_FACTOR, -10 - (actor->GetPosition().z - 0.5f));
        }

        if (actor->GetPosition().y < 1.0f)
        {
            collider->ResponceToEnvironment(Vector3(0, 1, 0), RESTITUTION_FACTOR, 1.0f - actor->GetPosition().y);
        }


        
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

void PhysicsManager::GetNextPosition(SphereColliderComponent* sphere, float elapsed_time)
{
    sphere->IntersectToEnvironment(RESTITUTION_FACTOR + 0.2f);
}

//--------------------------------------------------------------------------------------------------------------------------------

PhysicsManager::~PhysicsManager()
{
}

//--------------------------------------------------------------------------------------------------------------------------------
