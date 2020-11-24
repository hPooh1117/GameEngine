#pragma once
#include "./Utilities/Vector.h"

class SubView
{
private:
	Vector3 mPos;
	Vector3 mTarget;
	Vector3 mUp;
	float   mAspect;
	float   mFov;
	float   mNear;
	float   mFar;
	DirectX::XMFLOAT4X4 mViewMat;
	DirectX::XMFLOAT4X4 mProjMat;

	float mWidth;
	float mHeight;
	
	bool mbView;

public:
	SubView();
	~SubView();

	void Set(const Vector3& pos, const Vector3& target, const Vector3& up);
	void SetProjection(float fov, float aspect, float nearZ, float farZ);
	void SetOrtho(float fov, float aspect, float nearZ, float farZ);

	void Activate();

	const auto& GetView() { return mViewMat; }
	const auto& GetProj() { return mProjMat; }
	const auto& GetPos() { return mPos; }
	const auto& GetTarget() { return mTarget; }
};