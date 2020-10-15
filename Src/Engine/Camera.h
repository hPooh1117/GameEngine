#pragma once
#include <DirectXMath.h>
#include <wrl\client.h>
#include <d3d11.h>

#include "./Utilities/Point.h"
#include "./Utilities/Matrix.h"

class Camera
{
protected:
    Vector3 mStartPos;
    Vector3 mPos;
    Vector3 mTarget;
    Vector3 mForward;
    Vector3 mUp;

    float mFov = 0.0f;
    float mAspect = 0.0f;
    float mNear = 0.0f;
    float mFar = 0.0f;
    float mWidth = 0.0f;
    float mHeight = 0.0f;
    

    Quaternion mRotation;

public:
    Camera(const Vector3& pos) :mStartPos(pos), mPos(pos), mTarget({ 0, 0, 0 }), mUp({ 0, 1, 0 })
    {
    }

    virtual void Update(float elapsed_time) = 0;


    inline void Set(const Vector3& pos, const Vector3& target, const Vector3& up)
    {
        mPos = pos;
        mTarget = target;
        mUp = up;
    }

    inline void SetPosition(const Vector3& pos)
    {
        mPos = pos;
    }

    inline Vector3& getPosition()
    {
        return mPos;
    }

    inline Vector3& getTargetVector()
    {
        return mTarget;
    }
    inline Vector3& getForwardVector()
    {
        mForward = Vector3::Normalize(mTarget - mPos);
        return mForward;
    }
    inline Vector3& getUpVector()
    {
        return mUp;
    }
    inline float getNearZ() { return mNear; }
    inline float getFarZ() { return mFar; }

    virtual ~Camera() {}
};

