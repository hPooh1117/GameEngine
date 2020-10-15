#pragma once
#include "Camera.h"

#include "./Renderer/D3D_Helper.h"

class OrthoView : public Camera
{
public:
	OrthoView(const Vector3& pos);
	virtual void Update(float elapsed_time);

	void SetOrtho(float width, float height, float min, float max);
	//virtual DirectX::XMMATRIX getViewProj(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context);
	void SetPosition(const Vector3& pos);
	DirectX::XMMATRIX GetView();
	
	//virtual DirectX::XMMATRIX getViewInverse();
	//virtual DirectX::XMMATRIX getProjInverse(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context);
	DirectX::XMMATRIX GetProjection(D3D::DeviceContextPtr& imm_context);

	~OrthoView() = default;
};