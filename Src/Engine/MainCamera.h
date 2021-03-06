#pragma once
#include "Camera.h"

#include "./Renderer/D3D_Helper.h"

class MainCamera final : public Camera
{
private:
    Vector3 mForward;

    float mDeltaTime = 0.0f;

    bool m_bEnableMoving = true;

public:
    MainCamera(Vector3 pos);

    virtual void Update(float elapsed_time);

private:
    void SwitchMoveEnable();
    void MoveCamera();
    void ResetCameraPosition();
public:
    void SetFocusPoint(const Vector3& pos);
    

    inline Vector3& GetForwardVector() { return mForward; }
    inline bool GetMoveability() { return m_bEnableMoving; }

    ~MainCamera() = default;
};