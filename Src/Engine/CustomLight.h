#pragma once
#pragma once
#include "Light.h"

#include <array>

#include "UIClient.h"

class CustomLight : public Light, public UIClient
{
private:
	//--CONSTANTS
	static constexpr int SPOT_MAX = 4;
	static constexpr int POINT_MAX = 4;
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

	struct PointLightData
	{
		float index = 0.0f;
		float range = 0.0f;
		float type = 0.0f;
		float dummy = 0.0f;
		DirectX::XMFLOAT4 pos = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT4 color = { 0.6f, 0.6f, 0.6f, 1.0f };
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
		PointLightData m_point_lights[POINT_MAX];
		SpotLightData m_spot_lights[SPOT_MAX];
	};

private:
	//--VARIABLES
	Vector4 mLightDirection = {};
	float mAngle = DirectX::XM_PI;
	Vector4 mAmbientColor = { 0.1f, 0.1f, 0.1f, 0 };
	Vector4 mLightColor = { 0.7f, 0.7f, 0.7f, 1.0f };
	float mTime = 0.0f;
	std::array<SpotLightData, SPOT_MAX> mSpotLightData;
	std::array<PointLightData, POINT_MAX> mPointLightData;
public:
	CustomLight(D3D::DevicePtr& p_device);
	virtual void Update(float elapsed_time) override;
	virtual void Set(D3D::DeviceContextPtr& p_imm_context, const std::shared_ptr<CameraController>& p_camera) override;
	virtual void RenderUI() override;

	void SetLightDirection(const Vector3& dir);
	void SetLightColor(const Vector3& color);

	void SetSpotData(
		unsigned int index,
		const Vector3& pos,
		const Vector4& color,
		const Vector3& dir, 
		float range, 
		float near_area,
		float far_area);
	void SetPointData(unsigned int index, 
		const Vector3& pos,
		const Vector4& color, 
		float range);
	void SetSpotPosition(unsigned int index, Vector3& pos);
	void SetPointPosition(unsigned int index, Vector3& pos);
	void SetSpotDirection(unsigned int index, Vector3& dir);
	void SetSpotInValid(unsigned int index);
	void SetPointInValid(unsigned int index);
	
	virtual ~CustomLight() {}
};

inline void CustomLight::SetLightDirection(const Vector3& dir)
{
	mLightDirection = dir;
}