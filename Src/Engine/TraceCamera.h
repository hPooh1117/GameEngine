#pragma once
#include <memory>
#include "Camera.h"
#include "UIClient.h"

class Actor;

class TraceCamera : public Camera, public UIClient
{
private:
	static const Vector3 DISTANCE;
	static constexpr float LOCAL_Y_OFFSET = 20.0f;
	static constexpr float LOCAL_Z_OFFSET = -35.0f;


private:
	std::shared_ptr<Actor> m_pTargetActor;
	float mDeltaTime = 0.0f;
	Vector3 mForward = Vector3(0, 0, 1);

public:
	TraceCamera();

	virtual void update(float elapsed_time);
	void SetTarget(const std::shared_ptr<Actor>& target);
	void ResetCameraPosition();
	void SetFocusPoint(const Vector3& pos);

	virtual void RenderUI() override;
	//virtual DirectX::XMMATRIX getViewProj(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context);
	//virtual DirectX::XMMATRIX getViewInverse();


	virtual ~TraceCamera();
};