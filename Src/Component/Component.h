#pragma once
#include <memory>
#include <iostream>

class Actor;

class Component : public std::enable_shared_from_this<Component>
{
protected:
    //std::shared_ptr<Actor> m_pOwner;
    std::weak_ptr<Actor> m_pOwner;
    #define OwnerPtr    (m_pOwner.lock())

    bool mIsValid = true;
protected:
    Component(const std::shared_ptr<Actor>& owner):m_pOwner(owner) {}
public:
    template<class T>
    static std::shared_ptr<T> Initialize(const std::shared_ptr<Actor>& p_owner)
    {
        return std::shared_ptr<T>(new T(p_owner));
    }
    virtual bool Create() = 0;
    virtual void Destroy() = 0;
    virtual void Update(float elapsed_time) = 0;
    void BeInvalid() { mIsValid = false; }
    void BeValid() { mIsValid = true; }
    bool GetIsValid() { return mIsValid; }
    virtual ~Component()
    {}
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