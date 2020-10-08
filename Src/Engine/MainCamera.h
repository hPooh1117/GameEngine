#pragma once
#include "Camera.h"

#include "./Renderer/D3D_Helper.h"

class MainCamera final : public Camera
{
private:
    Vector3 mForward;

    float mDeltaTime = 0.0f;

    bool m_bEnableMoving = false;

public:
    MainCamera(Vector3 pos);
    virtual void update(float elapsed_time);

private:
    void SwitchMoveEnable();
    void MoveCamera();
    void ResetCameraPosition();
public:
    void SetFocusPoint(const Vector3& pos);
    

    inline Vector3& getForwardVector() { return mForward; }


    ~MainCamera() = default;
};