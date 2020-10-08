#include "CCPlayerCollector.h"
#include "./Application/Input.h"

#include "./Utilities/MyArrayFromVector.h"

#include "./Engine/Actor.h"
#include "./Engine/CustomLight.h"

//----------------------------------------------------------------------------------------------------------------------------

CCPlayerCollector::CCPlayerCollector(const std::shared_ptr<Actor>& owner)
:Component(owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<CCPlayerCollector> CCPlayerCollector::Initialize(const std::shared_ptr<Actor>& owner)
{
    return std::shared_ptr<CCPlayerCollector>(new CCPlayerCollector(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool CCPlayerCollector::Create()
{
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerCollector::Destroy()
{

}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerCollector::Update(float elapsed_time)
{
}


//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerCollector::RenderUI()
{

}

//----------------------------------------------------------------------------------------------------------------------------
