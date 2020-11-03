#include "DirectionalLight.h"
#include "./Application/Input.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "CameraController.h"
#include "./Utilities/misc.h"
#include "./Utilities/MyArrayFromVector.h"


DirectionalLight::DirectionalLight(D3D::DevicePtr& p_device)
    :
    Light(),
    mLightDirection({ 0, 0, 0, 0 }), 
    mAngle(DirectX::XM_PI),
    mEnvironmentMapAlpha(0.5f)
{
    auto result = p_device->CreateBuffer(
        &CD3D11_BUFFER_DESC(
            sizeof(CBufferForLight),
            D3D11_BIND_CONSTANT_BUFFER,
            D3D11_USAGE_DEFAULT,
            0,
            0,
            0
        ),
        nullptr,
        m_pConstantBuffer.GetAddressOf()
    );
    _ASSERT_EXPR_A(SUCCEEDED(result), hr_trace(result));


}

void DirectionalLight::Update(float elapsed_time)
{

    if (InputPtr.OnKeyDown("Left"))
    {
        mAngle += 0.001745f * 20;
    }
    if (InputPtr.OnKeyDown("Right"))
    {
        mAngle -= 0.001745f * 20;
    }

    mLightDirection.x = -sinf(mAngle);
    mLightDirection.y = -0.5f;
    mLightDirection.z = -cosf(mAngle);

    float d = sqrtf(mLightDirection.x * mLightDirection.x + mLightDirection.y * mLightDirection.y + mLightDirection.z * mLightDirection.z);
    if (d > 0) mLightDirection /= d;


}

void DirectionalLight::Set(D3D::DeviceContextPtr& p_imm_context, const std::shared_ptr<CameraController>& p_camera)
{
    CBufferForLight cb = {};
    cb.m_ambient_color = mAmbientColor;
    cb.m_eye_pos = Vector4(p_camera->GetCameraPosition());
    cb.m_light_color = mLightColor;
    cb.m_light_dir = mLightDirection;
    cb.m_tess = mTessFactor;
    cb.m_env_alpha = mEnvironmentMapAlpha;

    p_imm_context->UpdateSubresource(m_pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
    p_imm_context->VSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
    p_imm_context->HSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
    p_imm_context->DSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
    p_imm_context->GSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
    p_imm_context->PSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
}

void DirectionalLight::RenderUI()
{
    using namespace ImGui;
    MyArrayFromVector dir = MyArrayFromVector(mLightDirection);
    SliderFloat3("Light Direction x", dir.SetArray(), 0.0f, 5.0f);

    MyArrayFromVector color = MyArrayFromVector(mLightColor);
    SliderFloat4("Light Color", color.SetArray(), 0.0f, 1.0f);

    MyArrayFromVector ambColor = MyArrayFromVector(mAmbientColor);
    SliderFloat3("Ambient Color", ambColor.SetArray(), 0.0f, 1.0f);

    SliderFloat("TessFactor", &mTessFactor, 0.0f, 10.0f);

    SliderFloat("Environment Factor", &mEnvironmentMapAlpha, 0.0f, 1.0f);
}

DirectionalLight::~DirectionalLight()
{
}
