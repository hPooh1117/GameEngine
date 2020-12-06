#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include <memory>
#include "./Utilities/Vector.h"


class MoveComponent : public Component
{
protected:
    const Vector3 G = { 0, -9.8f, 0 };

protected:
    Vector3 mVelocity = {};
    Vector3 mAcceleration = {};
    Vector3 mResultant = {};
    float   mMass = 50000.0f;
    bool    mbOnGround = false;

public:
    MoveComponent(Actor* owner) :Component(owner) {}
    virtual ~MoveComponent() = default;


    virtual bool Create() = 0;
    virtual void Destroy() = 0;

    virtual void Update(float elapsed_time) = 0;

    static constexpr int GetID() { return ComponentID::kMove; }

    inline void SetVelocity(const Vector3& velocity) { mVelocity = velocity; }
    inline Vector3& GetVelocity() { return mVelocity; }

    inline void SetMass(float mass) { mMass = mass; }
    inline float GetMass() { return mMass; }

    void IsOnGround(bool flag) { mbOnGround = flag; }
};

