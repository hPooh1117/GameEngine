#include "ActorManager.h"

#include "Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

ActorManager::ActorManager()
{
}

//----------------------------------------------------------------------------------------------------------------------------

Actor* ActorManager::AddActor(int type)
{
    Actor* actor = new Actor();
    actor->Initialize(type);
    m_pActors.emplace(type, std::unique_ptr<Actor>(actor));
    return actor;
}

//----------------------------------------------------------------------------------------------------------------------------

void ActorManager::AddActor(Actor* actor, int id)
{
    m_pActors.emplace(id, std::unique_ptr<Actor>(actor));
}

//----------------------------------------------------------------------------------------------------------------------------

Actor* ActorManager::GetActor(int type)
{
    return m_pActors.find(type)->second.get();
}

//----------------------------------------------------------------------------------------------------------------------------

void ActorManager::Update(float elapsed_time)
{
    if (m_pActors.empty()) return;

    for (auto& actor :  m_pActors)
    {
        actor.second->Update(elapsed_time);
    }
}

void ActorManager::ClearAll()
{
    m_pActors.clear();
}

//----------------------------------------------------------------------------------------------------------------------------

ActorManager::~ActorManager()
{
}

//----------------------------------------------------------------------------------------------------------------------------
