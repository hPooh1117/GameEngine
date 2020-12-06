#include "TraceCamera.h"
#include "Actor.h"
#include "./Application/Input.h"
#include "./Utilities/MyArrayFromVector.h"

const Vector3 TraceCamera::DISTANCE = Vector3(0, 4, -20);

using namespace DirectX;
//--------------------------------------------------------------------------------------------------------------------------------

TraceCamera::TraceCamera() : Camera(DISTANCE)
{
	mNear = 0.1f;
	mFar = 1000.0f;
}

//--------------------------------------------------------------------------------------------------------------------------------

void TraceCamera::Update(float elapsed_time)
{
	mDeltaTime = elapsed_time;
	
	Matrix R = Matrix::CreateFromQuaternion(m_pTargetActor->GetQuaternion());
	Vector3 forward = R.GetZAxis();
	Vector3 up = R.GetYAxis();

	mTarget = m_pTargetActor->GetPosition();

	mPos = mTarget + forward * LOCAL_Z_OFFSET + up * LOCAL_Y_OFFSET;
	SetFocusPoint(mTarget);
}

//--------------------------------------------------------------------------------------------------------------------------------

void TraceCamera::SetTarget(Actor* target)
{
	if (m_pTargetActor != nullptr) return;

	m_pTargetActor = target;
}


//--------------------------------------------------------------------------------------------------------------------------------

void TraceCamera::ResetCameraPosition()
{
	if (InputPtr.OnKeyTrigger("1"))
	{
		mPos = mStartPos;

	}
}

//--------------------------------------------------------------------------------------------------------------------------------

void TraceCamera::SetFocusPoint(const Vector3& pos)
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

void TraceCamera::RenderUI()
{
	using namespace ImGui;

	SetNextTreeNodeOpen(false, ImGuiCond_Once);
	std::string title = "Trace Camera";

	if (TreeNode(title.data()))
	{
		MyArrayFromVector pos = MyArrayFromVector(mPos);
		SliderFloat3("Position", pos.SetArray(), -1000.0f, 1000.0f);

		MyArrayFromVector forward = MyArrayFromVector(mForward);
		SliderFloat3("Forward", forward.SetArray(), -2.0f, 2.0f);

		MyArrayFromVector up = MyArrayFromVector(mUp);
		SliderFloat3("Up", up.SetArray(), 0.0f, 1.0f);

		MyArrayFromVector rotation = MyArrayFromVector(mRotation);
		SliderFloat4("Quaternion", rotation.SetArray(), -10.0f, 10.0f);


		TreePop();
	}

}


//--------------------------------------------------------------------------------------------------------------------------------

TraceCamera::~TraceCamera()
{
}

//--------------------------------------------------------------------------------------------------------------------------------
