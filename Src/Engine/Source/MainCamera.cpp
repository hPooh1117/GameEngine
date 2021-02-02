#include "MainCamera.h"

#include "./Application/Input.h"
#include "./Application/Helper.h"


#include "./Utilities/Log.h"
#include "./Utilities/ImGuiSelf.h"
using namespace DirectX;


MainCamera::MainCamera(Vector3 pos) :Camera(pos)
{
	SetFocusPoint(Vector3(0, -2, 0));
	mNear = 0.1f;
	mFar = 1000.0f;
}

void MainCamera::Update(float elapsed_time)
{
	mDeltaTime = elapsed_time;

	// move camera position and orientation
	SwitchMoveEnable();

	ResetCameraPosition();

	/*if (m_bEnableMoving) */MoveCamera();
}


void MainCamera::SwitchMoveEnable()
{
	if (InputPtr.OnKeyTrigger("B"))
	{
		m_bEnableMoving = !m_bEnableMoving;
	}
}

void MainCamera::MoveCamera()
{

	float speed = 5.0f;
	Vector2 deltaMouse = InputPtr.GetMouseDelta();
	float pitch = 0.0f, yaw = 0.0f;

	if (m_bEnableMoving && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		// マウスMボタン押下 & マウス移動でカメラ平行移動
		if ((GetKeyState(VK_MBUTTON) & 0x8000)/* && (deltaMouse.x != 0 && deltaMouse.y != 0)*/)
		{
			Vector3 delta3D = Vector3(-deltaMouse.x, deltaMouse.y, 0);
			mForward.normalize();
			Vector3 Y = Vector3(0, 1, 0);
			Vector3 X = Y.cross(mForward);
			X.normalize();
			Vector3 Z = X.cross(Y);
			Z.normalize();

			Vector3 delta;
			delta.x = delta3D.x * X.x + delta3D.y * Y.x + delta3D.z * Z.x;
			delta.y = delta3D.x * X.y + delta3D.y * Y.y + delta3D.z * Z.y;
			delta.z = delta3D.x * X.z + delta3D.y * Y.z + delta3D.z * Z.z;
			mPos = MathOp::Lerp(mPos, mPos + delta * 0.06f, 0.8f);
		}

		// マウスホイールでカメラの前後平行移動
		if (InputPtr.GetIsMouseWheelUp())
		{
			mPos += mForward * speed;
		}
		if (InputPtr.GetIsMouseWheelDown())
		{
			mPos -= mForward * speed;
		}

		// マウス右クリック中のマウス移動で視野操作
		if (InputPtr.IsConnected(0))    // ゲームパッド操作
		{
			pitch = InputPtr.GetThumbRYValue() * mDeltaTime * -0.4f;
			yaw = InputPtr.GetThumbRXValue() * mDeltaTime * 0.4f;
		}
		else                             // マウス操作
		{
			if (GetAsyncKeyState(VK_RBUTTON) < 0)
			{
				pitch = deltaMouse.y * mDeltaTime * 0.1f;
				yaw = deltaMouse.x * mDeltaTime * 0.1f;
			}
		}

	}


	// カメラ視野移動の計算--------------------------------------
	Quaternion q1, q2;

	// 左右の軸はY軸
	q2.SetToRotateY(yaw);
	//q = Quaternion::Concatenate(q2, q1);
	mRotation = Quaternion::Concatenate(mRotation, q2);
	Matrix r;
	r = Matrix::CreateFromQuaternion(mRotation);

	// 前方向、上方向の設定
	mForward = Vector3(r._31, r._32, r._33);
	mUp = Vector3(r._21, r._22, r._23);

	// 上下の軸はカメラの右向き
	q1.SetToRotateAxis(mUp.cross(mForward), pitch);
	mRotation = Quaternion::Concatenate(mRotation, q1);

	mTarget = mPos + mForward;
	// ------------------------------------------------------
}

void MainCamera::ResetCameraPosition()
{
	if (InputPtr.OnKeyTrigger("1"))
	{
		mPos = mStartPos;
	}
}

void MainCamera::SetFocusPoint(const Vector3& pos)
{
	Quaternion q;
	Vector3 axis;
	float angle;


	Vector3 focus = Vector3::Normalize(pos - mPos);

	angle = acosf(Vector3::Dot(mForward, focus));
	axis = mForward.cross(focus);

	if (angle > 0.001f * 0.001745f || angle < -0.001f * 0.001745f)
	{

		q.SetToRotateAxis(axis, angle);

		mRotation = Quaternion::Concatenate(mRotation, q);
	}

	Matrix r;
	r = Matrix::CreateFromQuaternion(mRotation);

	mForward = Vector3(r._31, r._32, r._33);
	mUp = Vector3(r._21, r._22, r._23);
}

