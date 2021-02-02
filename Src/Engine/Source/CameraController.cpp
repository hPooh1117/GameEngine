#include "CameraController.h"

#include "Settings.h"
#include "MainCamera.h"
#include "TraceCamera.h"
#include "OrthoView.h"
#include "Actor.h"
#include "GameSystem.h"
#include "UIRenderer.h"


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

    mpCameras[kMoveable] = std::make_unique<MainCamera>(Vector3(0, 10, -50));
    mpCameras[kTrace] = std::make_unique<TraceCamera>();

    mpOrthoView = std::make_unique<OrthoView>(Vector3(0, 0, 0));
    mpOrthoView->SetOrtho(mOrthoWidth, mOrthoHeight, mOrthoNear, mOrthoFar);

    mNearPlane = mpCameras[kMoveable]->GetNearZ();
    mFarPlane = mpCameras[kTrace]->GetFarZ();

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
//#ifdef _DEBUG
//    if (InputPtr.OnKeyTrigger("C"))
//    {
//        mPrevMode = mCurrentMode;
//        if (++mCurrentMode >= kNumMax) mCurrentMode = 0;
//        mbIsBlended = true;
//        mBlendingTime = 0; // ブレンディング経過時間の初期化
//        NotifyCurrentMode();
//    }
//#endif

    //if (mbIsBlended) // ブレンド中なら
    //{
    //    // 前回カメラも更新
    //    mpCameras[mPrevMode]->Update(elapsed_time);

    //    // ブレンド経過時間を更新
    //    mBlendingTime++;

    //    // ブレンド時間Maxを超えればブレンド終了
    //    if (mBlendingTime >= mBlendingTimeMax) mbIsBlended = false;
    //}

    // UI表示中でImGuiウィンドウ上にマウスカーソルがある場合、更新しない。
    //if (ENGINE.GetUIRenderer()->GetUIEnable() && ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) return;

    // 現在のカメラを更新
    mpCameras[mCurrentMode]->Update(elapsed_time);

}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::SetMatrix(D3D::DeviceContextPtr& p_imm_context)
{
    // 現在のカメラ
    auto& itForCurrent = mpCameras[mCurrentMode];

    Vector3 pos;
    Vector3 target;
    Vector3 up;

    //if (mbIsBlended) // ブレンド中なら
    //{
    //    auto& itForPrev = mpCameras[mPrevMode];

    //    float t = static_cast<float>(mBlendingTime) / static_cast<float>(mBlendingTimeMax);

    //    pos = MathOp::Lerp(itForPrev->GetPosition(), itForCurrent->GetPosition(), t);
    //    target = MathOp::Lerp(itForPrev->GetTargetVector(), itForCurrent->GetTargetVector(), t);
    //    up = MathOp::Lerp(itForPrev->GetUpVector(), itForCurrent->GetUpVector(), t);
    //}
    //else
    //{
        pos = itForCurrent->GetPosition();
        target = itForCurrent->GetTargetVector();
        up = itForCurrent->GetUpVector();
    //}

    // カメラから情報をもらう
    mCameraPos = pos;
    mCameraTarget = target;
    mCameraUp = up;

    // ビュー行列更新
    CreateViewMatrix();

    // 射影行列更新
    CreateProjectionMatrix(p_imm_context);

    mpOrthoView->SetOrtho(mOrthoWidth, mOrthoHeight, mOrthoNear, mOrthoFar);
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::CreateViewMatrix()
{
    XMVECTOR eye = XMLoadFloat3(&mCameraPos);
    XMVECTOR target = XMLoadFloat3(&mCameraTarget);
    XMVECTOR up = XMLoadFloat3(&mCameraUp);

    XMMATRIX view = DirectX::XMMatrixLookAtLH(eye, target, up);

    XMStoreFloat4x4(&mView, view);
    
    XMMATRIX inv = DirectX::XMMatrixInverse(nullptr, view);
    XMStoreFloat4x4(&mInvView, inv);
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::CreateProjectionMatrix(D3D::DeviceContextPtr& p_imm_context)
{
    auto& it = mpCameras[mCurrentMode];

    D3D11_VIEWPORT viewport = {};
    UINT numOfViewPorts = 1;
    p_imm_context->RSGetViewports(&numOfViewPorts, &viewport);

    XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(30.0f * 0.01745f, viewport.Width / viewport.Height, mNearPlane, mFarPlane);

    XMStoreFloat4x4(&mProjection, proj);

    XMMATRIX inv = DirectX::XMMatrixInverse(nullptr, proj);
    XMStoreFloat4x4(&mInvProj, inv);
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
    //XMMATRIX view = XMLoadFloat4x4(&mView);
    //XMMATRIX inverse = XMMatrixInverse(nullptr, view);
    //return inverse;
    return XMLoadFloat4x4(&mInvView);
}

//--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX CameraController::GetProjMatrix(D3D::DeviceContextPtr& p_imm_context)
{
    return XMLoadFloat4x4(&mProjection);
}

//--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX CameraController::GetInvProjMatrix(D3D::DeviceContextPtr& p_imm_context)
{
    return XMLoadFloat4x4(&mInvProj);
}

DirectX::XMMATRIX CameraController::GetInvProjViewMatrix(D3D::DeviceContextPtr& p_imm_context)
{
    //*** (VP)^-1 = P^-1 * V^-1 ***

    //XMMATRIX P = XMLoadFloat4x4(&mProjection); 
    //XMMATRIX V = XMLoadFloat4x4(&mView);
    //XMMATRIX VP = XMMatrixMultiply(V, P);
    //XMMATRIX V_P = V * P;
    //XMMATRIX InvVP = XMMatrixInverse(nullptr, V * P);
    //XMMATRIX R = VP * InvVP;
    //XMVECTOR v = XMVectorSet(0, 3, 0, 1);
    //XMVECTOR v1 = XMVector4Transform(v, InvVP);
    //XMVECTOR v2 = XMVector4Transform(v, XMMatrixTranspose(InvVP));
    //return InvVP;
    return DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&mInvProj), DirectX::XMLoadFloat4x4(&mInvView));
}

//--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX CameraController::GetOrthoView()
{
    return mpOrthoView->GetView();
}

//--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX CameraController::GetOrthoProj(D3D::DeviceContextPtr& p_imm_context)
{
    return mpOrthoView->GetProjection(p_imm_context);
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::SetTarget(Actor* p_actor)
{
    Camera* camera = mpCameras.at(kTrace).get();
    static_cast<TraceCamera*>(camera)->SetTarget(p_actor);

    mpCameras[kMoveable]->SetPosition(Vector3(0, 10, -50));
    SetFocusPointOfMovableCamera(p_actor->GetPosition());
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::SetOrthoPos(Vector3 pos)
{
    mpOrthoView->Set(pos, Vector3(0, 0, 0), Vector3(0, 1, 0));
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::SetPositionOfMoveableCamera(const Vector3& pos)
{
    mpCameras[kMoveable]->SetPosition(pos);
}

void CameraController::SetFocusPointOfMovableCamera(const Vector3& pos)
{
    Camera* camera = mpCameras.at(kMoveable).get();
    static_cast<MainCamera*>(camera)->SetFocusPoint(pos);
}

//--------------------------------------------------------------------------------------------------------------------------------

void CameraController::RenderUI()
{
    using namespace ImGui;

    if (TreeNode("Camera"))
    {

        Text("Current State : %s", mCurrentMode == kMoveable ? "Moveable" : "Trace");

        Text("Blending : %d", mbIsBlended);




        MyArrayFromVector pos = MyArrayFromVector(mCameraPos);
        SliderFloat3("Camera Pos", pos.SetArray(), 0.0f, 1.0f);

        MyArrayFromVector target = MyArrayFromVector(mCameraTarget);
        SliderFloat3("Camera Target", target.SetArray(), 0.0f, 1.0f);

        MyArrayFromVector up = MyArrayFromVector(mCameraUp);
        SliderFloat3("Camera Up", up.SetArray(), 0.0f, 1.0f);


        MyArrayFromVector oPos = MyArrayFromVector(mpOrthoView->GetPosition());
        SliderFloat3("Ortho Camera Pos", oPos.SetArray(), -1000.0f, 1000.0f);

        SliderFloat("Ortho Width", &mOrthoWidth, 1.0f, 2000.0f);
        SliderFloat("Ortho Height", &mOrthoHeight, 1.0f, 1000.0f);
        SliderFloat("Ortho Near", &mOrthoNear, 0.1f, 10.0f);
        SliderFloat("Ortho Far", &mOrthoFar, 10.0f, 1000.0f);

        TreePop();
    }

    // Test View & Projeci
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
