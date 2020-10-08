#include "StaticComponent.h"

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<StaticComponent> StaticComponent::Initialize(const std::shared_ptr<Actor>& owner)
{
    return std::shared_ptr<StaticComponent>(new StaticComponent(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool StaticComponent::Create()
{
    mStartPos = m_pOwner.lock()->GetPosition();
    mMass = 5.0e10f;
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void StaticComponent::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void StaticComponent::Update(float elapsed_time)
{
    m_pOwner.lock()->SetPosition(mStartPos);
}

//----------------------------------------------------------------------------------------------------------------------------
