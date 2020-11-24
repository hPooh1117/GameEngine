#include "SubView.h"
#include "./Application/Helper.h"

SubView::SubView():
	mPos({0,0,0}),
	mTarget({ 0.0f, 1.0f, 0.0f }),
	mUp({0,1,0}),
	mAspect(1.0f),
	mFov(0.0f),
	mWidth(SCREEN_WIDTH),
	mHeight(SCREEN_HEIGHT),
	mNear(0.1f),
	mFar(1000.0f)
{
}

SubView::~SubView()
{
}

void SubView::Set(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	mPos = pos;
	mTarget = target;
	mUp = up;
}

void SubView::SetProjection(float fov, float aspect, float nearZ, float farZ)
{
	mbView = true;
	mFov = fov;
	mAspect = aspect;
	mNear = nearZ;
	mFar = farZ;
}

void SubView::SetOrtho(float fov, float aspect, float nearZ, float farZ)
{
	mbView = false;
	mFov = fov;
	mAspect = aspect;
	mNear = nearZ;
	mFar = farZ;
}

void SubView::Activate()
{
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&mPos);
	DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&mTarget);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&mUp);

	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
	DirectX::XMStoreFloat4x4(&mViewMat, view);

	DirectX::XMMATRIX proj;
	if (mbView) proj = DirectX::XMMatrixPerspectiveFovLH(mFov, mAspect, mNear, mFar);
	else        proj = DirectX::XMMatrixOrthographicLH(mWidth, mHeight, mNear, mFar);
	DirectX::XMStoreFloat4x4(&mProjMat, proj);
}
