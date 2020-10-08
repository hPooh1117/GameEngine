#pragma once
#include "Light.h"
#include <array>
#include "UIClient.h"

class SpotLight : public Light, public UIClient
{
private:
	//--CONSTANTS
	static constexpr int SPOT_MAX = 32;

private:
	//--DATA FORMAT
	struct SpotLightData
	{
		float index = 0.0f;
		float range = 0.0f;
		float type = 0.0f;
		float inner_corn = 0.0f;
		float outer_corn = 0.0f;
		float dummy0 = 0.0f;
		float dummy1 = 0.0f;
		float dummy2 = 0.0f;
		DirectX::XMFLOAT4 pos = {};
		DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
		DirectX::XMFLOAT4 dir = {};
	};

	struct CBufferForLight
	{
		DirectX::XMFLOAT4 m_light_color;
		DirectX::XMFLOAT4 m_light_dir;
		DirectX::XMFLOAT4 m_ambient_color;
		DirectX::XMFLOAT4 m_eye_pos;
		float m_env_alpha = 0.0f;
		float m_refract = 0.0f;
		float m_time = 0.0f;
		float m_dummy = 0.0f;
		SpotLightData m_spot_lights[SPOT_MAX];
	};

private:
	//--VARIABLES
	Vector4 mLightDirection = {};
	float mAngle = DirectX::XM_PI;
	Vector4 mAmbientColor = { 0.1f, 0.1f, 0.1f, 0 };
	Vector4 mLightColor = { 0.3f, 0.3f, 0.3f, 1.0f };
	float mTime = 0.0f;
	std::array<SpotLightData, SPOT_MAX> mSpotLightData;
public:
	SpotLight(D3D::DevicePtr& p_device);
	virtual void Update(float elapsed_time) override;
	virtual void Set(D3D::DeviceContextPtr& p_imm_context, const std::shared_ptr<CameraController>& p_camera) override;
	virtual void RenderUI() override;

	void SetData(
		const unsigned int   index, 
		const Vector3&       pos,
		const Vector4&       color,
		const Vector3&       dir, 
		const float          range, 
		const float          near_area, 
		const float          far_area);

	void SetPosition(unsigned int index, const Vector3& pos);
	void SetDirection(unsigned int index, const Vector3& dir);
	virtual ~SpotLight() {}
};