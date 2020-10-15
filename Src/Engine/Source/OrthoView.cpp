#include "OrthoView.h"

using namespace DirectX;


OrthoView::OrthoView(const Vector3& pos) :Camera(pos)
{
	mNear = 1.0f;
	mFar = 100.0f;
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

//DirectX::XMMATRIX OrthoView::getViewProj(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context)
//{
//    XMVECTOR vPos, vTarget, vUp;
//    vPos = XMVectorSet(mPos.x, mPos.y, mPos.z, 0);
//    vTarget = XMVectorSet(mTarget.x, mTarget.y, mTarget.z, 0);
//    vUp = XMVectorSet(mUp.x, mUp.y, mUp.z, 0);
//
//    XMMATRIX view = XMMatrixLookAtLH(vPos, vTarget, vUp);
//    
//    XMMATRIX proj = XMMatrixOrthographicLH(mWidth, mHeight, mNear, mFar);
//
//    return view * proj;
//}
//
//DirectX::XMMATRIX OrthoView::getView()
//{
//    return DirectX::XMMATRIX();
//}
//
//DirectX::XMMATRIX OrthoView::getViewInverse()
//{
//    XMVECTOR eye = XMLoadFloat3(&mPos);
//    XMVECTOR target = XMLoadFloat3(&mTarget);
//    XMVECTOR up = XMLoadFloat3(&mUp);
//
//    XMMATRIX view = XMMatrixLookAtLH(eye, target, up);
//    XMFLOAT4X4 view2;
//    XMStoreFloat4x4(&view2, view);
//    view2._41 = 0.0f;
//    view2._42 = 0.0f;
//    view2._43 = 0.0f;
//    view2._44 = 1.0f;
//
//    XMMATRIX inverse = XMLoadFloat4x4(&view2);
//    inverse = XMMatrixInverse(nullptr, inverse);
//
//    return inverse;
//
//}
//
//DirectX::XMMATRIX OrthoView::getProjInverse(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context)
//{
//    return DirectX::XMMATRIX();
//}
//
//DirectX::XMMATRIX OrthoView::getProjection(D3D::DeviceContextPtr& imm_context)
//{
//    return DirectX::XMMATRIX();
//}


