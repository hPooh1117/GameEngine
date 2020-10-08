#include "PointLight.h"
#include "./Application/Input.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "CameraController.h"
#include "./Utilities/misc.h"
#include "./Utilities/MyArrayFromVector.h"
#include <string>

PointLight::PointLight(D3D::DevicePtr& p_device):Light(), UIClient()
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

void PointLight::Update(float elapsed_time)
{
    if (InputPtr->OnKeyDown("Left"))
    {
        mAngle += 0.001745f * 20;
    }
    if (InputPtr->OnKeyDown("Right"))
    {
        mAngle -= 0.001745f * 20;
    }

    mLightDirection.x = -sinf(mAngle);
    mLightDirection.y = -0.5f;
    mLightDirection.z = -cosf(mAngle);

    float d = sqrtf(mLightDirection.x * mLightDirection.x + mLightDirection.y * mLightDirection.y + mLightDirection.z * mLightDirection.z);
    if (d > 0) mLightDirection /= d;

    mTime += elapsed_time;
}

void PointLight::Set(D3D::DeviceContextPtr& p_imm_context, const std::shared_ptr<CameraController>& p_camera)
{
    CBufferForLight cb = {};
    cb.m_ambient_color = mAmbientColor;
    cb.m_eye_pos = Vector4(p_camera->GetCameraPosition());
    cb.m_light_color = mLightColor;
    cb.m_light_dir = mLightDirection;
    cb.m_time = mTime;

    memcpy(cb.m_point_lights, mPointLightData.data(), sizeof(PointLightData) * LIGHT_NUM_MAX);

    p_imm_context->UpdateSubresource(m_pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
    p_imm_context->VSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
    p_imm_context->GSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
    p_imm_context->PSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());

}

void PointLight::RenderUI()
{
    using namespace ImGui;
    MyArrayFromVector dir = MyArrayFromVector(mLightDirection);
    SliderFloat3("Light Direction x", dir.SetArray(), 0.0f, 5.0f);

    MyArrayFromVector color = MyArrayFromVector(mLightColor);
    SliderFloat4("Light Color", color.SetArray(), 0.0f, 1.0f);

    MyArrayFromVector ambColor = MyArrayFromVector(mAmbientColor);
    SliderFloat3("Ambient Color", ambColor.SetArray(), 0.0f, 1.0f);

    for (auto i = 0; i < LIGHT_NUM_MAX; ++i)
    {
        SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
        std::string title = "Point Light " + std::to_string(i);

        if (TreeNode(title.data()))
        {
            Text("Index %.1f", mPointLightData[i].index);
            SliderFloat("Range", &mPointLightData[i].range, 0.0f, 50.0f);

            MyArrayFromVector pos = MyArrayFromVector(mPointLightData[i].pos);
            SliderFloat3("Position", pos.SetArray(), -10.0f, 10.0f);

            MyArrayFromVector pointColor = MyArrayFromVector(mPointLightData[i].color);
            SliderFloat4("Color", pointColor.SetArray(), 0.0f, 1.0f);

            TreePop();
        }
    }


}

void PointLight::SetData(unsigned int index, const Vector3& pos, const Vector4& color, float range)
{
    mPointLightData[index].pos = Vector4(pos);
    mPointLightData[index].index = static_cast<float>(index);
    mPointLightData[index].color = color;
    mPointLightData[index].range = range;
    mPointLightData[index].type = 1.0f;
}

void PointLight::SetPosition(unsigned int index, const Vector3& pos)
{
    if (mPointLightData[index].type == 0.0f) return;
    mPointLightData[index].pos = Vector4(pos);
}

PointLight::~PointLight()
{
}
