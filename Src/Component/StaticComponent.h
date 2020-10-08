#pragma once
#include "MoveComponent.h"

class StaticComponent : public MoveComponent
{
private:
    Vector3 mStartPos;

private:
    StaticComponent(const std::shared_ptr<Actor>& owner) :MoveComponent(owner) {}
    
public:
    virtual ~StaticComponent() {}

    static std::shared_ptr<StaticComponent> Initialize(const std::shared_ptr<Actor>& owner);
    virtual bool Create();
    virtual void Destroy();

    virtual void Update(float elapsed_time);

};