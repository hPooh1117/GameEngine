#pragma once
#include "MoveComponent.h"

class StaticComponent : public MoveComponent
{
private:
    Vector3 mStartPos;


public:
    StaticComponent(Actor* owner) :MoveComponent(owner) {}
    virtual ~StaticComponent() {}

    virtual bool Create();
    virtual void Destroy();

    virtual void Update(float elapsed_time);

};