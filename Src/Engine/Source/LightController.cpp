#include "LightController.h"

#include "DataStructures.h"
#include "CameraController.h"

#include "./Application/Input.h"

#include "./Utilities/Log.h"
#include "./Utilities/MyArrayFromVector.h"

//--------------------------------------------------------------------------------------------------------------------------------

LightController::LightController(D3D::DevicePtr& p_device)
	:mTessFactor(1.0f),
	mEnvironmentMapAlpha(0.5f),
	mTimer(0.0f),
	m_bSpotLightControllable(false),
	mLightDirection(0, 0, 1, 0),
	mYaw(DirectX::XM_PI),
	mPitch(30.0f)
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
	if (FAILED(result))
	{
		Log::Error("[LIGHT] Couldn't Create ConstantBuffer.");
	}

}

//--------------------------------------------------------------------------------------------------------------------------------

bool LightController::Init(unsigned int pointLightNum, unsigned int spotLightNum)
{

	if (!mPointLights.empty()) mPointLights.clear();
	if (!mSpotLights.empty()) mSpotLights.clear();

	mPointLights.resize(pointLightNum < NUM_POINT_LIGHT ? pointLightNum : NUM_POINT_LIGHT);
	mSpotLights.resize(spotLightNum < NUM_SPOT_LIGHT ? spotLightNum : NUM_SPOT_LIGHT);


	Log::Info("[LIGHT] Initialized.");
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::Update(float elapsed_time)
{
	UpdateDirectionalLights(elapsed_time);

	UpdateSpotLights(elapsed_time);

	mTimer += elapsed_time;
}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::UpdateDirectionalLights(float elapsed_time)
{
	if (InputPtr.OnKeyDown("Left"))
	{
		mYaw += 1.0f;
	}
	if (InputPtr.OnKeyDown("Right"))
	{
		mYaw -= 1.0f;
	}
	if (InputPtr.OnKeyDown("Up"))
	{
		mPitch += 1.0f;
	}
	if (InputPtr.OnKeyDown("Down"))
	{
		mPitch -= 1.0f;
	}

	//DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(mYaw * 0.01745f) * DirectX::XMMatrixRotationX(mPitch * 0.01745f);

	DirectX::XMFLOAT3 forward(0, 0, 1);
	//XMStoreFloat4(&mLightDirection, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&forward), rot));
	
	DirectX::XMVECTOR q = DirectX::XMQuaternionRotationRollPitchYaw(mPitch * 0.01745f, mYaw * 0.01745f, 0.0f);
	DirectX::XMMATRIX r = DirectX::XMMatrixRotationQuaternion(q);
	XMStoreFloat4(&mLightDirection, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&forward), r));


	//mLightDirection.x = -sinf(mYaw);
	//mLightDirection.y = -0.25f;
	//mLightDirection.z = -cosf(mYaw);

	//float d = sqrtf(mLightDirection.x * mLightDirection.x + mLightDirection.z * mLightDirection.z);
	//if (d > 0)
	//{
	//	mLightDirection.x /= d;
	//	mLightDirection.z /= d;
	//}

}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::UpdateSpotLights(float elapsed_time)
{
	for (auto i = 0u; i < NUM_SPOT_LIGHT; ++i)
	{
		if (i >= mSpotLights.size()) break;

		if (mSpotLights[i].b_enable == false) continue;

		mSpotLights[i].dir.x = -sinf(mSpotLights[i].angle);
		mSpotLights[i].dir.y = mSpotLights[i].height;
		mSpotLights[i].dir.z = -cosf(mSpotLights[i].angle);
	}

	if (InputPtr.OnKeyTrigger("LeftAlt"))
	{
		m_bSpotLightControllable = !m_bSpotLightControllable;
		Log::Info("[LIGHT] Toggled SpotLightControllability. (%s)", m_bSpotLightControllable ? "True" : "False");
	}

	if (!m_bSpotLightControllable)
	{
		if (mSpotLights.size() >= 4)
		{
			static float angle = DirectX::XM_PI / 4;
			angle += elapsed_time;
			float s = sinf(angle) * 1.0f;
			float c = cosf(angle) * 1.0f;

			mSpotLights[0].dir = Vector3(-s, -1, c);
			mSpotLights[1].dir = Vector3(-c, -1, s);
			mSpotLights[2].dir = Vector3(s, -1, -c);
			mSpotLights[3].dir = Vector3(c, -1, -s);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::ActivatePointLight(unsigned int index, bool b_activate)
{
	mPointLights[index].b_enable = b_activate;
}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::ActivateSpotLight(unsigned int index, bool b_activate)
{
	mSpotLights[index].b_enable = b_activate;
}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::SendLightData(
	PointLightGPU& pointData,
	SpotLightGPU& spotLightData,
	DirectilnalLightGPU& dirctionalData)
{
	// TODO: ライト情報を送って、よそで定数バッファを管理したい。
}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::SetLightColor(const Vector4& color)
{
	mLightColor = color;
}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::SetPointData(
	unsigned int       index,
	const Vector3& pos,
	const Vector4& color,
	float              range)
{
	if (index >= NUM_POINT_LIGHT)
	{
		Log::Warning("Couldn't Set PointLight Data(No.%d). It's OUT OF RANGE.", index);
		return;
	}
	if (index >= mPointLights.size())
	{
		Log::Warning("PointLight Vector was Refered OUT OF RANGE (Vector Size : %d, Refered : %d)", mPointLights.size(), index);
	}


	mPointLights.at(index).b_enable = true;
	mPointLights.at(index).index = index;
	mPointLights.at(index).pos = pos;
	mPointLights.at(index).color = color;
	mPointLights.at(index).range = range;
}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::SetSpotData(
	unsigned int       index,
	const Vector3& pos,
	const Vector4& color,
	const Vector3& direction,
	float              range,
	float              nearCorn,
	float              farCorn)
{
	if (index >= NUM_SPOT_LIGHT)
	{
		Log::Warning("Couldn't Set SpotLight Data(No.%d). It's OUT OF RANGE.", index);
		return;
	}
	if (index >= mSpotLights.size())
	{
		Log::Warning("SpotLight Vector was Refered OUT OF RANGE (Vector Size : %d, Refered : %d)", mSpotLights.size(), index);
	}

	mSpotLights.at(index).b_enable = true;
	mSpotLights.at(index).index = index;
	mSpotLights.at(index).pos = pos;
	mSpotLights.at(index).color = color;
	mSpotLights.at(index).dir = direction;
	mSpotLights.at(index).inner_corn = nearCorn;
	mSpotLights.at(index).outer_corn = farCorn;
	mSpotLights.at(index).range = range;
}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::SetDataForGPU(D3D::DeviceContextPtr& pImmContext, CameraController* pCamera)
{
	CBufferForLight cb = {};
	cb.directionalLight.ambient_color = mAmbientColor;
	cb.directionalLight.light_color = mLightColor;
	cb.directionalLight.light_direction = mLightDirection;
	cb.directionalLight.eye_pos = Vector4(pCamera->GetCameraPosition());
	cb.directionalLight.env_alpha = mEnvironmentMapAlpha;
	cb.directionalLight.tess_factor = mTessFactor;
	cb.directionalLight.time = mTimer;

	for (auto i = 0u; i < NUM_POINT_LIGHT; ++i)
	{
		if (i >= mPointLights.size()) break;

		PointLightData& point = mPointLights.at(i);
		cb.pointLight[i].index = static_cast<float>(point.index);
		cb.pointLight[i].range = point.range;
		cb.pointLight[i].type = point.b_enable ? 1.0f : 0.0f;
		cb.pointLight[i].pos = point.pos;
		cb.pointLight[i].color = point.color;
	}

	for (auto j = 0u; j < NUM_SPOT_LIGHT; ++j)
	{
		if (j >= mSpotLights.size()) break;

		SpotLightData& spot = mSpotLights.at(j);
		cb.spotLight[j].index = static_cast<float>(spot.index);
		cb.spotLight[j].range = spot.range;
		cb.spotLight[j].type = spot.b_enable ? 1.0f : 0.0f;
		cb.spotLight[j].inner_corn = spot.inner_corn;
		cb.spotLight[j].outer_corn = spot.outer_corn;
		cb.spotLight[j].pos = spot.pos;
		cb.spotLight[j].color = spot.color;
		cb.spotLight[j].dir = spot.dir;
	}


	pImmContext->UpdateSubresource(m_pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);

	pImmContext->VSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
	pImmContext->GSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());
	pImmContext->PSSetConstantBuffers(2, 1, m_pConstantBuffer.GetAddressOf());

}

//--------------------------------------------------------------------------------------------------------------------------------

void LightController::RenderUI()
{
	using namespace ImGui;

	SliderFloat("X-Angle", &mPitch, -180.0f, 180.0f);
	SliderFloat("Y-Angle", &mYaw, -180.0f, 180.0f);

	
	Text("Direction : %.2f, ", mLightDirection.x);
	SameLine();
	Text("%.2f, ", mLightDirection.y);
	SameLine();
	Text("%.2f", mLightDirection.z);

	MyArrayFromVector color = MyArrayFromVector(mLightColor);
	SliderFloat4("Light Color", color.SetArray(), 0.0f, 1.0f);

	MyArrayFromVector ambColor = MyArrayFromVector(mAmbientColor);
	SliderFloat3("Ambient Color", ambColor.SetArray(), 0.0f, 1.0f);

	SliderFloat("Environment Factor", &mEnvironmentMapAlpha, 0.0f, 1.0f);

	SliderFloat("TessFactor", &mTessFactor, 0.0f, 10.0f);

	for (auto i = 0u; i < NUM_POINT_LIGHT; ++i)
	{
		if (i >= mPointLights.size()) break;

		SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		std::string title = "Point Light " + std::to_string(i);

		if (TreeNode(title.data()))
		{
			RadioButton("Enable", mPointLights[i].b_enable);
			if (mPointLights[i].b_enable == false) continue;

			Text("Index %d", mPointLights[i].index);
			SliderFloat("Range", &mPointLights[i].range, 0.0f, 50.0f);

			MyArrayFromVector pos = MyArrayFromVector(mPointLights[i].pos);
			SliderFloat3("Position", pos.SetArray(), -10.0f, 10.0f);

			MyArrayFromVector pointColor = MyArrayFromVector(mPointLights[i].color);
			SliderFloat4("Color", pointColor.SetArray(), 0.0f, 1.0f);

			TreePop();
		}

	}

	Text("Spot Light Controllable : %s", m_bSpotLightControllable ? "True" : "False");

	for (auto j = 0u; j < NUM_SPOT_LIGHT; ++j)
	{
		if (j >= mSpotLights.size()) break;

		SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		std::string title = "Spot Light " + std::to_string(j);

		if (TreeNode(title.data()))
		{
			RadioButton("Enable", mSpotLights[j].b_enable);
			if (mSpotLights[j].b_enable == false) continue;

			Text("Index %.1f", mSpotLights[j].index);
			SliderFloat("Range", &mSpotLights[j].range, 0.0f, 50.0f);

			MyArrayFromVector pos = MyArrayFromVector(mSpotLights[j].pos);
			SliderFloat3("Position", pos.SetArray(), -10.0f, 10.0f);

			MyArrayFromVector spotColor = MyArrayFromVector(mSpotLights[j].color);
			SliderFloat4("Color", spotColor.SetArray(), 0.0f, 1.0f);

			SliderFloat("Angle", &mSpotLights[j].angle, -DirectX::XM_PI, DirectX::XM_PI);

			SliderFloat("Height", &mSpotLights[j].height, -1.0f, 0.0f);

			TreePop();
		}

	}
	Separator();
}

//--------------------------------------------------------------------------------------------------------------------------------
