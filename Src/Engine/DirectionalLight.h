#pragma once
#include "Light.h"
#include "UIClient.h"

class Camera;

class DirectionalLight : public Light, public UIClient
{
private:
    struct CBufferForLight
    {
        DirectX::XMFLOAT4 m_light_color;
        DirectX::XMFLOAT4 m_light_dir;
        DirectX::XMFLOAT4 m_ambient_color;
        DirectX::XMFLOAT4 m_eye_pos;
        float m_env_alpha = 0.0f;
        float m_refract = 0.0f;
        float m_time = 0.0f;
        float m_tess = 0.0f;
    };

private:
    Vector4 mLightDirection;
    float mAngle;
    Vector4 mAmbientColor = { 0.1f, 0.1f, 0.1f, 0 };
    Vector4 mLightColor = { 0.8f, 0.8f, 0.8f, 1.0f };
    float mTessFactor = 1.0f;
    float mEnvironmentMapAlpha;

public:
    DirectionalLight(D3D::DevicePtr& p_device);
    virtual void Update(float elapsed_time) override;
    virtual void Set(D3D::DeviceContextPtr& p_imm_context, const std::shared_ptr<CameraController>& p_camera) override;
    virtual void RenderUI() override;

    inline Vector4& GetLightDirection() { return mLightDirection; }
    virtual ~DirectionalLight();
};