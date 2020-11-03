#include "CustomLight.h"
#include "./Application/Input.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "CameraController.h"
#include "./Utilities/misc.h"
#include "./Utilities/MyArrayFromVector.h"
#include <string>


//--------------------------------------------------------------------------------------------------------------------------------

CustomLight::CustomLight(D3D::DevicePtr& p_device)
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

//--------------------------------------------------------------------------------------------------------------------------------

void CustomLight::Update(float elapsed_time)
{
    //if (InputPtr->OnKeyDown("Q"))
    //{
    //    mYaw += 0.001745f * 20;
    //}
    //if (InputPtr->OnKeyDown("E"))
    //{
    //    mYaw -= 0.001745f * 20;
    //}

    //mLightDirection.x = -sinf(mYaw);
    //mLightDirection.y = -0.5f;
    //mLightDirection.z = -cosf(mYaw);

    //float d = sqrtf(mLightDirection.x * mLightDirection.x + mLightDirection.y * mLightDirection.y + mLightDirection.z * mLightDirection.z);
    //if (d > 0) mLightDirection /= d;

    //mTime += elapsed_time;

}

//--------------------------------------------------------------------------------------------------------------------------------

void CustomLight::Set(D3D::DeviceContextPtr& p_imm_context, const std::shared_ptr<CameraController>& p_camera)
{
    CBufferForLight cb = {};
    cb.m_ambient_color = mAmbientColor;
    cb.m_eye_pos = Vector4(p_camera->GetCameraPosition());
    cb.m_light_color = mLightColor;
    cb.m_light_dir = mLightDirection;
    cb.m_time = mTime;

    memcpy(cb.m_spot_lights, mSpotLightData.data(), sizeof(SpotLightData) * SPOT_MAX);
    memcpy(cb.m_point_lights, mPointLightData.data(), sizeof(PointLightData) * POINT_MAX);
    p_imm_context->UpdateSubresource(m_pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
    p_imm_context->VSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
    p_imm_context->GSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
    p_imm_context->PSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());

}

//--------------------------------------------------------------------------------------------------------------------------------

inline void CustomLight::RenderUI()
{
    using namespace ImGui;
    MyArrayFromVector dir = MyArrayFromVector(mLightDirection);
    SliderFloat3("Light Direction", dir.SetArray(), 0.0f, 5.0f);

    MyArrayFromVector color = MyArrayFromVector(mLightColor);
    SliderFloat4("Light Color", color.SetArray(), 0.0f, 1.0f);

    MyArrayFromVector ambColor = MyArrayFromVector(mAmbientColor);
    SliderFloat3("Ambient Color", ambColor.SetArray(), 0.0f, 1.0f);

    for (auto i = 0; i < SPOT_MAX; ++i)
    {
        if (mSpotLightData[i].type == 0.0f) continue;

        SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
        std::string title = "Spot Light " + std::to_string(i);

        if (TreeNode(title.data()))
        {
            Text("Index %.1f", mSpotLightData[i].index);
            SliderFloat("Range", &mSpotLightData[i].range, 0.0f, 50.0f);

            MyArrayFromVector pos = MyArrayFromVector(mSpotLightData[i].pos);
            SliderFloat3("Position", pos.SetArray(), -10.0f, 10.0f);

            MyArrayFromVector spotColor = MyArrayFromVector(mSpotLightData[i].color);
            SliderFloat4("Color", spotColor.SetArray(), 0.0f, 1.0f);

            MyArrayFromVector dir = MyArrayFromVector(mSpotLightData[i].dir);
            SliderFloat3("Dir", dir.SetArray(), -1.0f, 1.0f);

            SliderFloat("Inner Corn", &mSpotLightData[i].inner_corn, 0.0f, 5.0f);

            SliderFloat("Outer Corn", &mSpotLightData[i].outer_corn, 0.0f, 5.0f);

            TreePop();
        }
    }

    for (auto i = 0; i < POINT_MAX; ++i)
    {
        if (mPointLightData[i].type == 0.0f) continue;

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

//--------------------------------------------------------------------------------------------------------------------------------

void CustomLight::SetLightColor(const Vector3& color)
{
    mLightColor = color;
}

void CustomLight::SetSpotData(
    unsigned int index, 
    const Vector3& pos, 
    const Vector4& color, 
    const Vector3& dir, 
    float range, 
    float near_area, 
    float far_area)
{
    if (index < 0) return;
    if (index >= SPOT_MAX) return;
    Vector3 lightDir = dir;
    lightDir.normalize();

    mSpotLightData[index].pos = Vector4(pos);
    mSpotLightData[index].index = static_cast<float>(index);
    mSpotLightData[index].color = color;
    mSpotLightData[index].range = range;
    mSpotLightData[index].type = 1.0f;
    mSpotLightData[index].dir = Vector4(lightDir);
    mSpotLightData[index].inner_corn = near_area;
    mSpotLightData[index].outer_corn = far_area;

}

//--------------------------------------------------------------------------------------------------------------------------------

void CustomLight::SetPointData(
    unsigned int index, 
    const Vector3& pos, 
    const Vector4& color, 
    float range)
{
    if (index < 0) return;
    if (index >= POINT_MAX) return;

    mPointLightData[index].pos = Vector4(pos);
    mPointLightData[index].index = static_cast<float>(index);
    mPointLightData[index].color = color;
    mPointLightData[index].range = range;
    mPointLightData[index].type = 1.0f;

}

//--------------------------------------------------------------------------------------------------------------------------------

void CustomLight::SetSpotPosition(unsigned int index, Vector3& pos)
{
    if (index < 0) return;
    if (index >= SPOT_MAX) return;
    if (mSpotLightData[index].type == 0.0f) return;

    mSpotLightData[index].pos = Vector4(pos);

}

//--------------------------------------------------------------------------------------------------------------------------------

void CustomLight::SetPointPosition(unsigned int index, Vector3& pos)
{
    if (index < 0) return;
    if (index >= SPOT_MAX) return;
    if (mPointLightData[index].type == 0.0f) return;

    mPointLightData[index].pos = Vector4(pos);

}

//--------------------------------------------------------------------------------------------------------------------------------

void CustomLight::SetSpotDirection(unsigned int index, Vector3& dir)
{
    if (index < 0) return;
    if (index >= SPOT_MAX) return;
    if (mSpotLightData[index].type == 0.0f) return;

    mSpotLightData[index].dir = Vector4(dir);

}

//--------------------------------------------------------------------------------------------------------------------------------

void CustomLight::SetSpotInValid(unsigned int index)
{
    mSpotLightData[index].type = 0.0f;
}

//--------------------------------------------------------------------------------------------------------------------------------

void CustomLight::SetPointInValid(unsigned int index)
{
    mPointLightData[index].type = 0.0f;
}

//--------------------------------------------------------------------------------------------------------------------------------
