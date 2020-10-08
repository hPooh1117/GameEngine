#include "ActorManager.h"


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

    for (unsigned int i = 0; i < m_pActors.size(); ++i)
    {
        auto it = m_pActors.find(i);
        if (it != m_pActors.end())
        it->second->Update(elapsed_time);
    }
}

//----------------------------------------------------------------------------------------------------------------------------

ActorManager::~ActorManager()
{
}

//----------------------------------------------------------------------------------------------------------------------------
