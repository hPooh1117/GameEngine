#include "MainCamera.h"

#include "./Application/Input.h"
#include "./Application/Helper.h"
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

	// set camera's direction vector
	//XMMATRIX origin = XMMatrixRotationRollPitchYaw(m_origin.x, m_origin.y, m_origin.z);
	//XMMATRIX R = XMLoadFloat4x4(&m_rotation);
	//R *= origin;
	//XMStoreFloat4x4(&m_rotation, /*R*/origin);
	//Matrix r;
	//r.CreateFromQuaternion(mRotation);
	//m_forward = Vector3(r._31, r._32, r._33);
	//m_up = Vector3(r._21, r._22, r._23);

	// move camera position and orientation
	SwitchMoveEnable();

	if (m_bEnableMoving) MoveCamera();

	ResetCameraPosition();
}


void MainCamera::SwitchMoveEnable()
{
	if (InputPtr->OnKeyTrigger("Space"))
	{
		m_bEnableMoving = !m_bEnableMoving;
	}
}

void MainCamera::MoveCamera()
{

	// WASDで前後左右移動
	if (InputPtr->OnKeyDown("W"))
	{
		mPos += mForward * 0.1f;
	}
	if (InputPtr->OnKeyDown("S"))
	{
		mPos -= mForward * 0.1f;
	}
	if (InputPtr->OnKeyDown("A"))
	{
		mPos -= mUp.cross(mForward) * 0.1f;
	}
	if (InputPtr->OnKeyDown("D"))
	{
		mPos += mUp.cross(mForward) * 0.1f;
	}

	// 左シフト、左コントロールでそれぞれ上移動、下移動
	if (InputPtr->OnKeyDown("LeftShift"))
	{
		mPos += Vector3(0, 1, 0) * 0.1f;
	}
	if (InputPtr->OnKeyDown("LeftCtrl"))
	{
		mPos -= Vector3(0, 1, 0) * 0.1f;
	}


	float pitch, yaw;
	if (InputPtr->IsConnected(0))    // ゲームパッド操作
	{
		pitch = InputPtr->GetThumbRYValue() * mDeltaTime * -0.4f;
		yaw = InputPtr->GetThumbRXValue() * mDeltaTime * 0.4f;
	}
	else                             // マウス操作
	{
		Vector2 deltaMouse = InputPtr->GetMouseDelta();

		pitch = deltaMouse.y * mDeltaTime * 0.1f;
		yaw = deltaMouse.x * mDeltaTime * 0.1f;
	}

	// カメラの上下に制限 
	if (pitch < -90.0f * 0.01745f) pitch = -90.0f * 0.01745f;
	if (pitch > 90.0f * 0.01745f) pitch = 90.0f * 0.01745f;


	Quaternion q1, q2;

	// 左右の軸はY軸
	q2.SetToRotateY(yaw);
	//q = Quaternion::Concatenate(q2, q1);
	mRotation = Quaternion::Concatenate(mRotation, q2);
	Matrix r;
	r = Matrix::CreateFromQuaternion(mRotation);

	mForward = Vector3(r._31, r._32, r._33);
	mUp = Vector3(r._21, r._22, r._23);

	// 上下の軸はカメラの右向き
	q1.SetToRotateAxis(mUp.cross(mForward), pitch);
	mRotation = Quaternion::Concatenate(mRotation, q1);

	mTarget = mPos + mForward;
}

void MainCamera::ResetCameraPosition()
{
	if (InputPtr->OnKeyTrigger("1"))
	{
		mPos = mStartPos;
	}
}

void MainCamera::SetFocusPoint(const Vector3& pos)
{
	Vector3 focus = pos - mPos;
	focus.normalize();

	Vector3 axis;
	axis = mForward.cross(focus);
	float angle = acosf(Vector3::Dot(mForward, focus));
	mRotation.SetToRotateAxis(axis, angle);
	//XMFLOAT3 axis2 = axis.getXMFLOAT3();
	//XMMATRIX mat = XMMatrixRotationAxis(XMLoadFloat3(&axis2), angle);
	//Matrix R;
	//R = Matrix::CreateFromQuaternion(mRotation);
	////XMStoreFloat4x4(&m_rotation, /*R * */mat);
	//m_forward = { R._31, R._32, R._33 };
	//m_up = { R._21, R._22, R._23 };
}

