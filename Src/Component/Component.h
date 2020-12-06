#pragma once
#include <memory>
#include <iostream>

class Actor;

class Component
{
protected:
    Actor* mpOwner;

    bool mIsValid = true;

public:

    Component(Actor* owner):mpOwner(owner) {}
    virtual bool Create() = 0;
    virtual void Destroy() = 0;
    virtual void Update(float elapsed_time) = 0;
    void BeInvalid() { mIsValid = false; }
    void BeValid() { mIsValid = true; }
    bool GetIsValid() { return mIsValid; }
    virtual ~Component() = default;
};



enum ComponentID
{
    kNone,
    kMove,
    kRender,

    kCollider,
    kSphereCollider,

    kCCPlayerMove,
    kCCEnemyMove,
    kCCPlayerCollector,
    kCCPlayerLightController,
    kCCTargetPulledUp,
    kUI,
    
    ID_NUM_MAX,
};