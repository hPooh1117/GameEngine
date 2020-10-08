#pragma once
#include "Light.h"

#include <array>

#include "UIClient.h"

class PointLight : public Light, public UIClient
{
public:
    static constexpr UINT LIGHT_NUM_MAX = 32;

private:
    struct PointLightData
    {
        float index = 0.0f;
        float range = 0.0f;
        float type = 0.0f;
        float dummy = 0.0f;
        DirectX::XMFLOAT4 pos = {0.0f, 0.0f, 0.0f, 1.0f};
        DirectX::XMFLOAT4 color = {0.6f, 0.6f, 0.6f, 1.0f};
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
        PointLightData m_point_lights[LIGHT_NUM_MAX];
    };


private:
    Vector4 mLightDirection = {};
    float mAngle = 0.0f;
    Vector4 mAmbientColor = { 0.1f, 0.1f, 0.1f, 0 };
    Vector4 mLightColor = { 0.8f, 0.8f, 0.8f, 1.0f };
    float mTime = 0.0f;
    std::array<PointLightData, LIGHT_NUM_MAX> mPointLightData;

public:
	PointLight(D3D::DevicePtr& p_device);

	virtual void Update(float elapsed_time);
	virtual void Set(D3D::DeviceContextPtr& p_imm_context, const std::shared_ptr<CameraController>& p_camera);
    virtual void RenderUI();

    void SetData(unsigned int index, const Vector3& pos, const Vector4& color, float range);
    void SetPosition(unsigned int index, const Vector3& pos);
	~PointLight();
};