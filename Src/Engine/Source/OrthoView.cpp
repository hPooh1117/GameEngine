#include "OrthoView.h"

using namespace DirectX;


OrthoView::OrthoView(const Vector3& pos) :Camera(pos)
{
	mNear = 1.0f;
	mFar = 1000.0f;
}

void OrthoView::Update(float elapsed_time)
{
}

void OrthoView::SetOrtho(float width, float height, float min, float max)
{
	mWidth = width;
	mHeight = height;
	mNear = min;
	mFar = max;
}

void OrthoView::SetPosition(const Vector3& pos)
{
	mPos = pos;
}

DirectX::XMMATRIX OrthoView::GetView()
{
	XMVECTOR vPos, vTarget, vUp;
	vPos = XMVectorSet(mPos.x, mPos.y, mPos.z, 0);
	vTarget = XMVectorSet(mTarget.x, mTarget.y, mTarget.z, 0);
	vUp = XMVectorSet(mUp.x, mUp.y, mUp.z, 0);

	XMMATRIX view = XMMatrixLookAtLH(vPos, vTarget, vUp);

	return view;
}

DirectX::XMMATRIX OrthoView::GetProjection(D3D::DeviceContextPtr& imm_context)
{
	XMMATRIX proj = XMMatrixOrthographicLH(mWidth, mHeight, mNear, mFar);

	return proj;
}

