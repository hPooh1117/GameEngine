#include "CameraController.h"

#include "Settings.h"
#include "MainCamera.h"
#include "TraceCamera.h"
#include "OrthoView.h"
#include "Actor.h"

#include "./Application/Input.h"
#include "./Application/Helper.h"

#include "./Utilities/Util.h"
#include "./Utilities/MyArrayFromVector.h"
#include "./Utilities/Log.h"

using namespace DirectX;



//--------------------------------------------------------------------------------------------------------------------------------

CameraController::CameraController()
    :mOrthoWidth(60),
    mOrthoHeight(45),
    mOrthoNear(0.1f),
    mOrthoFar(200.0f)
{
    mCameraNameTable[kMoveable] = "Moveable Camera";
    mCameraNameTable[kTrace] = "Trace Camera";

    m_pCameras[kMoveable] = std::make_shared<MainCamera>(Vector3(0, 10, -50));
    m_pCameras[kTrace] = std::make_shared<TraceCamera>();

    m_pOrthoView = std::make_shared<OrthoView>(Vector3(0, 0, 0));
    m_pOrthoView->SetOrtho(mOrthoWidth, mOrthoHeight, mOrthoNear, mOrthoFar);

    Log::Info("[CAMERA] Initialized.");
    NotifyCurrentMode();
}

//--------------------------------------------------------------------------------------------------------------------------------

bool CameraController::Initialize(Settings::Camera& cameraSettings)
{
    mFovWidth = cameraSettings.fovV;
    mFovHeight = cameraSettings.fovH;
    mAspect = cameraSettings.aspect;
    mNearPlane = cameraSettings.nearPlane;
    mFarPlane = cameraSettings.farPlane;
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::Update(float elapsed_time)
{
#ifdef _DEBUG
    if (InputPtr.OnKeyTrigger("C"))
    {
        mPrevMode = mCurrentMode;
        if (++mCurrentMode >= kNumMax) mCurrentMode = 0;
        m_bIsBlended = true;
        mBlendingTime = 0; // ブレンディング経過時間の初期化
        NotifyCurrentMode();
    }
#endif

    if (m_bIsBlended) // ブレンド中なら
    {
        // 前回カメラも更新
        m_pCameras[mPrevMode]->Update(elapsed_time);

        // ブレンド経過時間を更新
        mBlendingTime++;

        // ブレンド時間Maxを超えればブレンド終了
        if (mBlendingTime >= mBlendingTimeMax) m_bIsBlended = false;
    }

    // 現在のカメラを更新
    m_pCameras[mCurrentMode]->Update(elapsed_time);

}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::SetMatrix(D3D::DeviceContextPtr& p_imm_context)
{
    // 現在のカメラ
    auto& itForCurrent = m_pCameras[mCurrentMode];

    Vector3 pos;
    Vector3 target;
    Vector3 up;

    if (m_bIsBlended) // ブレンド中なら
    {
        auto& itForPrev = m_pCameras[mPrevMode];

        float t = static_cast<float>(mBlendingTime) / static_cast<float>(mBlendingTimeMax);

        pos = Math::Lerp(itForPrev->getPosition(), itForCurrent->getPosition(), t);
        target = Math::Lerp(itForPrev->getTargetVector(), itForCurrent->getTargetVector(), t);
        up = Math::Lerp(itForPrev->getUpVector(), itForCurrent->getUpVector(), t);
    }
    else
    {
        pos = itForCurrent->getPosition();
        target = itForCurrent->getTargetVector();
        up = itForCurrent->getUpVector();
    }

    // カメラから情報をもらう
    mCameraPos = pos;
    mCameraTarget = target;
    mCameraUp = up;

    // ビュー行列更新
    CreateViewMatrix();

    // 射影行列更新
    CreateProjectionMatrix(p_imm_context);

    m_pOrthoView->SetOrtho(mOrthoWidth, mOrthoHeight, mOrthoNear, mOrthoFar);
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::CreateViewMatrix()
{
    XMVECTOR eye = XMLoadFloat3(&mCameraPos);
    XMVECTOR target = XMLoadFloat3(&mCameraTarget);
    XMVECTOR up = XMLoadFloat3(&mCameraUp);

    XMMATRIX view = XMMatrixLookAtLH(eye, target, up);

    XMStoreFloat4x4(&mView, view);
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::CreateProjectionMatrix(D3D::DeviceContextPtr& p_imm_context)
{
    auto& it = m_pCameras[mCurrentMode];
    float nearZ = it->getNearZ();
    float farZ = it->getFarZ();

    D3D11_VIEWPORT viewport = {};
    UINT numOfViewPorts = 1;
    p_imm_context->RSGetViewports(&numOfViewPorts, &viewport);

    XMMATRIX proj = XMMatrixPerspectiveFovLH(30.0f * 0.01745f, viewport.Width / viewport.Height, nearZ, farZ);
    

    XMStoreFloat4x4(&mProjection, proj);
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::NotifyCurrentMode()
{
    Log::Info("[CAMERA] Current Mode : %s", mCameraNameTable[mCurrentMode].c_str());
}


//--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX CameraController::GetViewMatrix()
{
    return XMLoadFloat4x4(&mView);
}

//--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX CameraController::GetInvViewMatrix()
{
    Matrix view = mView;

    //view._41 = 0.0f;
    //view._42 = 0.0f;
    //view._43 = 0.0f;
    //view._44 = 1.0f;
    XMMATRIX inverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&view));
    return inverse;
}

//--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX CameraController::GetProjMatrix(D3D::DeviceContextPtr& p_imm_context)
{
    return XMLoadFloat4x4(&mProjection);
}

//--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX CameraController::GetInvProjMatrix(D3D::DeviceContextPtr& p_imm_context)
{
    XMMATRIX inverse = XMLoadFloat4x4(&mProjection);
    return XMMatrixInverse(nullptr, inverse);
}

DirectX::XMMATRIX CameraController::GetInvProjViewMatrix(D3D::DeviceContextPtr& p_imm_context)
{
    // (VP)^-1 = P^-1 * V^-1

    XMMATRIX P = XMLoadFloat4x4(&mProjection); 
    XMMATRIX V = XMLoadFloat4x4(&mView);
    return XMMatrixInverse(nullptr, V * P);
}

//--------------------------------------------------------------------------------------------------------------------------------

//const std::shared_ptr<Camera>& CameraController::GetCameraPtr(unsigned int mode)
//{
//    if (mode < 0 || mode >= CameraMode::kNumMax)
//    {
//        Log::Error("[CAMERA] Refered Nullptr(Mode : %d). Max Number of Camera Mode is %d", mode, CameraMode::kNumMax);
//    }
//
//    return m_pCameras.at(mode);
//}

//--------------------------------------------------------------------------------------------------------------------------------


DirectX::XMMATRIX CameraController::GetOrthoView()
{
    return m_pOrthoView->GetView();
}

//--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX CameraController::GetOrthoProj(D3D::DeviceContextPtr& p_imm_context)
{
    return m_pOrthoView->GetProjection(p_imm_context);
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::SetTarget(std::shared_ptr<Actor>& p_actor)
{
    std::shared_ptr<Camera> camera = m_pCameras.at(kTrace);
    std::static_pointer_cast<TraceCamera>(camera)->SetTarget(p_actor);

    m_pCameras[kMoveable]->SetPosition(Vector3(0, 10, -50));
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::SetOrthoPos(Vector3 pos)
{
    m_pOrthoView->Set(pos, Vector3(0, 0, 0), Vector3(0, 1, 0));
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::SetPositionOfMoveableCamera(const Vector3& pos)
{
    m_pCameras[kMoveable]->SetPosition(pos);
}

void CameraController::SetFocusPointOfMovableCamera(const Vector3& pos)
{
    std::shared_ptr<Camera> camera = m_pCameras.at(kMoveable);
    std::static_pointer_cast<MainCamera>(camera)->SetFocusPoint(pos);
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::RenderUI()
{
    using namespace ImGui;

    if (TreeNode("Camera"))
    {

        Text("Current State : %s", mCurrentMode == kMoveable ? "Moveable" : "Trace");

        Text("Blending : %d", m_bIsBlended);




        MyArrayFromVector pos = MyArrayFromVector(mCameraPos);
        SliderFloat3("Camera Pos", pos.SetArray(), 0.0f, 1.0f);

        MyArrayFromVector target = MyArrayFromVector(mCameraTarget);
        SliderFloat3("Camera Target", target.SetArray(), 0.0f, 1.0f);

        MyArrayFromVector up = MyArrayFromVector(mCameraUp);
        SliderFloat3("Camera Up", up.SetArray(), 0.0f, 1.0f);


        MyArrayFromVector oPos = MyArrayFromVector(m_pOrthoView->getPosition());
        SliderFloat3("Ortho Camera Pos", oPos.SetArray(), -1000.0f, 1000.0f);

        SliderFloat("Ortho Width", &mOrthoWidth, 1.0f, 2000.0f);
        SliderFloat("Ortho Height", &mOrthoHeight, 1.0f, 1000.0f);
        SliderFloat("Ortho Near", &mOrthoNear, 0.1f, 10.0f);
        SliderFloat("Ortho Far", &mOrthoFar, 10.0f, 1000.0f);

        TreePop();
    }

    //Text("view0 : %.2f, %.2f, %.2f, %.2f", mView._11, mView._12, mView, mView._14);
    //Text("view1 : %.2f, %.2f, %.2f, %.2f", mView._21, mView._22, mView, mView._24);
    //Text("view2 : %.2f, %.2f, %.2f, %.2f", mView._31, mView._32, mView, mView._34);
    //Text("view3 : %.2f, %.2f, %.2f, %.2f", mView._41, mView._42, mView, mView._44);

    //Text("Proj0 : %.2f, %.2f, %.2f, %.2f", mProjection._11, mProjection._12, mProjection._13, mProjection._14);
    //Text("Proj1 : %.2f, %.2f, %.2f, %.2f", mProjection._21, mProjection._22, mProjection._23, mProjection._24);
    //Text("Proj2 : %.2f, %.2f, %.2f, %.2f", mProjection._31, mProjection._32, mProjection._33, mProjection._34);
    //Text("Proj3 : %.2f, %.2f, %.2f, %.2f", mProjection._41, mProjection._42, mProjection._43, mProjection._44);

    //XMMATRIX proj = XMLoadFloat4x4(&mProjection);
    //XMMATRIX inverseP = XMMatrixInverse(nullptr, proj);
    //XMFLOAT4X4 invP; XMStoreFloat4x4(&invP, inverseP);

    //XMMATRIX I = inverseP * proj;
    //XMFLOAT4X4 i; XMStoreFloat4x4(&i, I);
    //Text("InvProj0 : %.2f, %.2f, %.2f, %.2f", invP._11, invP._12, invP._13, invP._14);
    //Text("InvProj1 : %.2f, %.2f, %.2f, %.2f", invP._21, invP._22, invP._23, invP._24);
    //Text("InvProj2 : %.2f, %.2f, %.2f, %.2f", invP._31, invP._32, invP._33, invP._34);
    //Text("InvProj3 : %.2f, %.2f, %.2f, %.2f", invP._41, invP._42, invP._43, invP._44);

    //Text("mulview : %.2f, %.2f, %.2f, %.2f", i._11, i._12, i._13, i._14);
    //Text("mulview : %.2f, %.2f, %.2f, %.2f", i._21, i._22, i._23, i._24);
    //Text("mulview : %.2f, %.2f, %.2f, %.2f", i._31, i._32, i._33, i._34);
    //Text("mulview : %.2f, %.2f, %.2f, %.2f", i._41, i._42, i._43, i._44);
    Separator();
}

//--------------------------------------------------------------------------------------------------------------------------------
