#include "ActorManager.h"

#include "Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

ActorManager::ActorManager()
{
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<Actor> ActorManager::AddActor(int type)
{
    std::shared_ptr<Actor> actor;
    actor->Initialize(type);
    m_pActors.emplace(type, actor);
    return actor;
}

//----------------------------------------------------------------------------------------------------------------------------

void ActorManager::AddActor(std::shared_ptr<Actor>& actor)
{
    m_pActors.emplace(actor->GetID(), actor);
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<Actor>& ActorManager::GetActor(int type)
{
    return m_pActors.find(type)->second;
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
